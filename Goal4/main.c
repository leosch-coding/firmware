#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "adc.h"

// Sets up our global-logic variables
uint32_t tick_counter = 0;
uint32_t offset = 0;
uint32_t on_or_off = 0;
uint16_t total = 0;
uint16_t current_sample = 0;
uint16_t samples[16];
int sample_index = 0;
int sample_count = 0;
int previous_was_above = 0;

// Objective 4+ specific state
int injector_state = 0;
int injection_index = 0;
int added_injection = 0;
int current_button_state = 1;
int previous_button_state = 1;

// *Important*
// This state is what main checks
// 0 == no output
// 1 == not enough history
// 2 == enough history, no detection
// 3 == enough history, detection
volatile int output = 0;



// Sets up some important register pointers
volatile uint32_t *GPIOA_MODER = (uint32_t *)0x40020000;
volatile uint32_t *GPIOA_ODR = (uint32_t *)0x40020014;
volatile uint32_t *GPIOC_MODER = (uint32_t *)0x40020800;
volatile uint32_t *GPIOC_IDR = (uint32_t *)0x40020810;


volatile uint32_t *TIM2_SR = (uint32_t *)0x40000010;

void counter(void) {
    tick_counter++;

    // Check if the button is currently being pressed
    if ((*GPIOC_IDR & (1 << 13)) == 0) {

       // If so, set the current button state to pressed (0 in this case represents true for a button press)
       current_button_state = 0;
    } else {

       // if it is not, set the current button state to false
       current_button_state = 1;

    }
    
    
    // check if there's a change in button state
    if (current_button_state != previous_button_state) {

       // check if the button state is 0 (meaning that the button has been pressed) 
       // because previous button state HAS to be 1, we know that the button has been fully pressed
       // set injector state to 1
       if (current_button_state == 0) {
          injector_state = 1;
          injection_index = 0;
       }
    }


    // store the current button state for the next loop 
    previous_button_state = current_button_state;
    
    // 0 == divisible by ten true
    // !0 == divisible by ten false
    int tenth_tick_state_check = tick_counter % 10;

    // Checks if we've reached 10 ticks yet. If so, add 1 to the conversion counter
    if (tenth_tick_state_check == 0 && injector_state == 1) {
       switch (injection_index) {
          case 0:
            added_injection = 80;
            injection_index++;
            break;
          case 1:
            added_injection = 60;
            injection_index++;
            break;
          case 2:
            added_injection = 45;
            injection_index++;
            break;
          case 3:
            added_injection = 33;
            injection_index++;
            break;
          case 4:
            added_injection = 24;
            injection_index++;
            break;
          case 5:
            added_injection = 18;
            injection_index++;
            break;
          case 6:
            added_injection = 13;
            injection_index++;
            break;
          case 7:
            added_injection = 9;
            injector_state = 0;
       }
    } 

    if (tenth_tick_state_check == 0) {
          adc_start();
          // wait until adc_ready returns true
          while (adc_ready() != true) {  
          }
            // stores our current sample into a variable to be used
            current_sample = adc_read();
    }
   
    if (added_injection > 0) {
       current_sample += added_injection;
    }

    // checks if we've gotten past 16 adc-checks
    if (sample_count == 16) {
          // finds the accumulation of the samples so far
          total = samples[0] + samples[1] + samples[2] + samples[3] + samples[4] + samples[5] + samples[6] + samples[7] + samples[8] + samples[9] + samples[10] + samples[11] + samples[12] + samples[13] + samples[14] + samples[15];    

          // calculates the noise floor by finding the mean
          uint32_t noise_floor = (total / 16);

          // calculates threshold by adding 12 to the mean/noise_floor
          uint32_t threshold = noise_floor + 12;

          // checks if the current sample is larger then the threshold
          if (current_sample > threshold) {
              // if the current sample is above the threshold, and the previous sample was above the threshold, trigger detection
              if (previous_was_above == 1) {
            
                output = 3;
        
              // if the current sample is above the threshold, but the previous sample was not, set previous_was_above to true
              } else if (previous_was_above == 0) {

                previous_was_above = 1;
                output = 2;
          
              // if neither the current sample nor the previous is above the threshold, reset previous_was_above to false
            } else {
            previous_was_above = 0;
            output = 2;
            }
          // checks if we've gotten two samples above the threshold
        } else {
          // Sample count is below 16
          // Set output state to 1
          output = 1;
        }
      }
    // moves the current sample into the index
    samples[sample_index] = current_sample;
    
    // resets added_injection to 0
    added_injection = 0;

    // checks if the sample index is 15 (index starts at 0)
    if (sample_index == 15) {

       // Resets the index
       sample_index = 0;

    } else {
       // Otherwise increments the index
       sample_index++;
    }
    
    // checks if sample count is below 16 yet
    if (sample_count < 16) {

       // if so, increment it UP UNTIL 16, then stop
       sample_count++;
    }  

    uint32_t elapsed = tick_counter - offset;

    if (elapsed >= 500) {
       offset += 500;
      
       if (on_or_off == 1) {
       
       *GPIOA_ODR &= ~(1 << 5);
       on_or_off = 0;
       
       } else {
       
       *GPIOA_ODR |= (1 << 5);
       on_or_off = 1;

       }
    }

    *TIM2_SR &= ~(1 << 0);
} 

void main(void) {
    // We make a pointer to RCC_APB1ENR
    volatile uint32_t *RCC_APB1ENR = (uint32_t *)0x40023840;

    // Use OR to set bit 0 to 1, giving TIM2 a clock
    *RCC_APB1ENR |= 0b1;

    volatile uint32_t *TIM2_PSC = (uint32_t *)0x40000028;
  
    // Move 15 into TIM2_PSC
    *TIM2_PSC = 15;

    volatile uint32_t *TIM2_ARR = (uint32_t *)0x4000002c;

    // Set the number of times to tick until interrupt to 999 (basically making it tick 1000 times)
    *TIM2_ARR = 999;

    volatile uint32_t *TIM2_DIER = (uint32_t *)0x4000000C;

    *TIM2_DIER |= 0b1;

    volatile uint32_t *NVIC_ISER0 = (uint32_t *)0xE000E100;

    *NVIC_ISER0 |= (1 << 28);

    volatile uint32_t *RCC_AHB1ENR = (uint32_t *)0x40023830;

    // Enables GPIOA's clock
    *RCC_AHB1ENR |= (1 << 0);

    // Enables GPIOC's clock
    *RCC_AHB1ENR |= (1 << 2);

    // Clears out and sets GPIOA's MODE to 01 (GP Output)
    *GPIOA_MODER &= ~(0x3 << 10);
    *GPIOA_MODER |= (1 << 10);

    // Sets GPIOC's MODE to 00 (input)
    *GPIOC_MODER &= ~(0x3 << 26);

    // Buttons current logic level
    // 0 == pressed
    // 1 == not-pressed
    *GPIOC_IDR |= (1 << 13);  

    // Initialize ADC
    adc_init();

    // We make a pointer to TIM2_CR1
    volatile uint32_t *TIM2_CR1 = (uint32_t *)0x40000000;

    // Use OR to set bit 0 to 1, enabling the counter
    *TIM2_CR1 |= 0b1;

    while (true) {
      if (output == 0) {
        // No output yet
        // Do nothing
        continue;

      } else if (output == 1) {
        // Output exists
        // Less then 16 historical samples
        printf("Noise-Floor: 0");
        printf("Threshold: 0");
        printf("Detection: 0");

        // reset output state
        output = 0;
      
      } else if (output == 2) {
        // Output exists, historical samples secured
        // No detection
        printf("PASS");

        // reset output state
        output = 0;

      } else if (output == 3) {
        // Output exists, historical samples secured
        // Detection
        printf("HIT");

        // reset output state
        output = 0;
      }
    }
}

