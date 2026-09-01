// Note: This code is ugly, I plan to fix it once I get the ADC driver set up

#include <stdint.h>
#include <stdbool.h>

uint32_t tick_counter = 0;
uint32_t offset = 0;
uint32_t on_or_off = 0;
volatile uint32_t *GPIOA_MODER = (uint32_t *)0x40020000;
volatile uint32_t *GPIOA_ODR = (uint32_t *)0x40020014;
volatile uint32_t *TIM2_SR = (uint32_t *)0x40000010;


void counter(void) {
    tick_counter++;
    uint32_t elapsed = tick_counter - offset;

    if (elapsed >= 500) {
       offset += 500;
      
       if (on_or_off == 1) {
       
       *GPIOA_ODR &= 0b11111111111111111111111111011111;
       on_or_off = 0;
       
       } else {
       
       *GPIOA_ODR |= 0b00000000000000000000000000100000;
       on_or_off = 1;

       }
    }

    *TIM2_SR &= 0b11111111111111111111111111111110;
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

    *RCC_AHB1ENR |= 0b1;
  
    *GPIOA_MODER &= 0b11111111111111111111001111111111; 
    *GPIOA_MODER |= 0b00000000000000000000010000000000;

    // We make a pointer to TIM2_CR1
    volatile uint32_t *TIM2_CR1 = (uint32_t *)0x40000000;

    // Use OR to set bit 0 to 1, enabling the counter
    *TIM2_CR1 |= 0b1;

    while (true) {

    }
}
