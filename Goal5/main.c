#include <stdint.h>
#include <stdbool.h>
#include "adc.h"

// Output
volatile uint32_t tick = 0;
volatile uint16_t adc_raw = 0;
volatile uint32_t noise = 0;
volatile uint32_t thresh = 0;
volatile int det = 0;
volatile uint32_t isr_us = 0; 
volatile uint32_t isr_max = 0;
volatile int missed = 0;


// Sets up our global-logic variables
uint32_t offset = 0;
uint32_t on_or_off = 0;
uint16_t total = 0;
uint16_t samples[16];
int sample_index = 0;
int sample_count = 0;
int previous_was_above = 0;
int injector_state = 0;
int injection_index = 0;
int added_injection = 0;
int current_button_state = 1;
int previous_button_state = 1;
int detect_state = 0;
volatile int initial_cycle_count = 0;
volatile int final_cycle_count = 0;
volatile int ready = 0;

// REGISTERS
volatile uint32_t *GPIOA_MODER = (uint32_t *)0x40020000;
volatile uint32_t *GPIOA_ODR = (uint32_t *)0x40020014;
volatile uint32_t *GPIOB_MODER = (uint32_t *)0x40020400;
volatile uint32_t *GPIOB_ODR = (uint32_t *)0x40020414;
volatile uint32_t *GPIOC_MODER = (uint32_t *)0x40020800;
volatile uint32_t *GPIOC_IDR = (uint32_t *)0x40020810;
volatile uint32_t *DWT_CTRL = (uint32_t *)0xE0001000;
volatile uint32_t *DWT_CYCCNT = (uint32_t *)0xE0001004;
volatile uint32_t *DEMCR = (uint32_t *)0xE000EDFC;
volatile uint32_t *TIM2_SR = (uint32_t *)0x40000010;

// AI-GENERATED: Specifically for UART
// Commented out later, as I do not know how UART works, and I would not be able to explain what it's doing properly
/*
volatile uint32_t *GPIOA_AFRL = (uint32_t *)0x40020020;
volatile uint32_t *RCC_APB1ENR = (uint32_t *)0x40023840;
volatile uint32_t *USART2_SR = (uint32_t *)0x40004400;
volatile uint32_t *USART2_DR = (uint32_t *)0x40004404;
volatile uint32_t *USART2_BRR = (uint32_t *)0x40004408;
volatile uint32_t *USART2_CR1 = (uint32_t *)0x4000440C;


 void uart_init(void) {
    // Enable clock
   *RCC_APB1ENR |= (1 << 17);

    // PA2 AF
    *GPIOA_MODER &= ~(3 << 4);
    *GPIOA_MODER |=  (2 << 4);

    // PA2 == AF7 (USART2) 
    *GPIOA_AFRL &= ~(0xF << 8);
    *GPIOA_AFRL |=  (7 << 8);

    // 115200 baud assuming 16 MHz peripheral clock
    *USART2_BRR = 139;

    // Enable transmitter and USART
    *USART2_CR1 |= (1 << 3);
    *USART2_CR1 |= (1 << 13);
}

void uart_putc(char c)
{
    while ((*USART2_SR & (1 << 7)) == 0)
    {
    }

    *USART2_DR = c;
}

void uart_puts(const char *s)
{
    while (*s)
    {
        uart_putc(*s++);
    }
}

void uart_put_u32(uint32_t value)
{
    char buffer[10];
    int i = 0;

    if (value == 0) {
        uart_putc('0');
        return;
    }

    while (value > 0) {
        buffer[i] = '0' + (value % 10);
        value /= 10;
        i++;
    }

    while (i > 0) {
        i--;
        uart_putc(buffer[i]);
    }
}

void uart_put_u32_width(uint32_t value, int width)
{
    char buffer[10];
    int i = 0;

    while (value > 0) {
        buffer[i] = '0' + (value % 10);
        value /= 10;
        i++;
    }

    while (i < width) {
        uart_putc('0');
        width--;
    }

    while (i > 0) {
        i--;
        uart_putc(buffer[i]);
    }
}
*/




void counter(void) {

    *TIM2_SR &= ~(1 << 0);

    // Drives PB3 high 
    *GPIOB_ODR |= (1 << 5);
    
    initial_cycle_count = *DWT_CYCCNT;

    // Increases the tick counter
    tick++;

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
    int tenth_tick_state_check = tick % 10;

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
          adc_raw = adc_read();
    }
   
    if (added_injection > 0) {
       adc_raw += added_injection;
    }

    // checks if we've gotten past 16 adc-checks
    if (sample_count == 16) {
          // finds the accumulation of the samples so far
          total = samples[0] + samples[1] + samples[2] + samples[3] + samples[4] + samples[5] + samples[6] + samples[7] + samples[8] + samples[9] + samples[10] + samples[11] + samples[12] + samples[13] + samples[14] + samples[15];    

          // calculates the noise floor by finding the mean
          noise = (total / 16);

          // calculates threshold by adding 12 to the mean/noise_floor
          thresh = noise + 12;

          // checks if the current sample is larger then the threshold
          if (adc_raw > thresh) {
              // if the current sample is above the threshold, and the previous sample was above the threshold, trigger detection
              if (previous_was_above == 1) {
            
                detect_state = 3;


              // if the current sample is above the threshold, but the previous sample was not, set previous_was_above to true
              } else if (previous_was_above == 0) {

                previous_was_above = 1;
                detect_state = 2;

              // if neither the current sample nor the previous is above the threshold, reset previous_was_above to false
            } else {
            previous_was_above = 0;
            detect_state = 2;

            }
          // checks if we've gotten two samples above the threshold
        } else {
          // Sample count is below 16
          // Set output state to 1
          detect_state = 1;
        }
      }
    // moves the current sample into the index
    samples[sample_index] = adc_raw;
    
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

    uint32_t elapsed = tick - offset;

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

    final_cycle_count = *DWT_CYCCNT;

    int uif_status = (*TIM2_SR & (1 << 0));

    if (uif_status == 1){
       missed++;
    }

    ready = 1;

    *TIM2_SR &= ~(1 << 0);

    // Drives PB5 low
    *GPIOB_ODR &= ~(1 << 5);

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

    // Enables GPIOB's clock
    *RCC_AHB1ENR |= (1 << 1);

    // Enables GPIOC's clock
    *RCC_AHB1ENR |= (1 << 2);

    // Clears out and sets GPIOA's MODE to 01 (GP Output)
    *GPIOA_MODER &= ~(0x3 << 10);
    *GPIOA_MODER |= (1 << 10);

    // Clears out and sets GPIOB's MODE to 01 (GP Output)
    *GPIOB_MODER &= (0x3 << 10);
    *GPIOB_MODER |= (1 << 10);

    // Sets GPIOC's MODE to 00 (input)
    *GPIOC_MODER &= ~(0x3 << 26);

    // Buttons current logic level
    // 0 == pressed
    // 1 == not-pressed
    *GPIOC_IDR |= (1 << 13);  

    // Enables TRCENA (which lets us use the cycle counter)
    *DEMCR |= (1 << 24); 

    // Enables the cycle counter
    *DWT_CTRL |= (1 << 0);


    // Initialize ADC
    adc_init();

    //**AI-GENERATED**
    //
    /*
    uart_init();

    uart_puts("HELLO\r\n");
    */
    // We make a pointer to TIM2_CR1
    volatile uint32_t *TIM2_CR1 = (uint32_t *)0x40000000;

    // Use OR to set bit 0 to 1, enabling the counter
    *TIM2_CR1 |= 0b1;

    while (true) {
      if (ready == 0) {
        // No output yet
        // Do nothing
        continue;

      } else if (detect_state == 3) {
        // Output exists
        // Less then 16 historical samples
        det = 1;
      } else {
        // Output exists, historical samples secured
        // No detection
        det = 0;
      } 
      
      // calculates isr_us
      uint32_t elapsed_cycles = final_cycle_count - initial_cycle_count; 
      isr_us = elapsed_cycles / 16;

      // finds isr_max
      if (isr_us > isr_max) {
        isr_max = isr_us;
      }

      // **AI-GENERATED**
      // Commented out as I do not know/understand UART, and do not want to present something I can't explain
      /*
      uart_put_u32_width(tick, 9);
      uart_putc(',');

      uart_put_u32_width(adc_raw, 4);
      uart_putc(',');

      uart_put_u32_width(noise, 4);
      uart_putc(',');

      uart_put_u32_width(thresh, 4);
      uart_putc(',');

      uart_put_u32(det);
      uart_putc(',');

      uart_put_u32(isr_us);
      uart_putc(',');

      uart_put_u32(isr_max);
      uart_putc(',');

      uart_put_u32(missed);
      uart_puts("\r\n");
      */
ready = 0; 
    }
}
