1. The clock is the default; HSI, which is 16MHz, I then used TIM2_PSC to divide by 16 - turning it to 1MHz.
   TIM2_ARR was used to force it to 'tick' 1000 times via setting it to 999.
   This leads to 1000Hz.
   I will be honest, I planned to find the frequency the night after I set this part up. I've only realized now that I completely forgot to do it after waking up.
   

2. I shared a large number of variables across the two functions:
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

I checked to make sure that neither of the two functions clobber a state tracker. I also used volatile to make sure that the compiler doesn't optimize away any information.

3. The detector/main.c calls the ADC driver functions, which return their values to be used by the caller.
   The actual, current ADC return value itself is stored in adc_raw.
   Now, about the edge case;
   If I am not mistaken, I have accounted for the fact that this can happen by forcing our counter into a while-loop for as long as adc_ready is not true.

4. I measured irs_us by using two global variables, initial_cycle_count and final_cycle_count. One taken at the beginning of function `count`, and one at the end.
   In `main`, I subtract final_cycle_count by initial_cycle_count to find elapsed_cycles.
   As our CPU cycles at 16MHz, 16 cycles = 1 μs, we find irs_us via taking our elapsed cycles and dividing by 16.

5. I did not reach requirement 7. If I were to try and force it to break, I would attempt using the real time clock to stall each time by enough to cause it to miss its deadline.
   If that did not work, and I needed to brute-force it, I would probably attempt next using a while-loop that counts to an absurdly high number to stall until it misses its deadline

6. I did not reach this section unfortunately.

7. I was not able to get it to output anything in time.
   To avoid overwhelming myself, I tackled one problem at a time.
   To avoid getting caught up on overengineering each step, I planned to build 3, 4, and 5 sequentially and smoothen out logical bugs after it compiled.
   Both of these strategies backfired when I realized that I could not use printf to easily see output. I did not have enough time to learn how UART learned from scratch like I could with the rest of the functionality.
   I used AI to generate now-commented out code to see what the output would've been.
   The outputs were mostly correct, but my tick counter was off. This is something I'm researching deeper into.
   

8. Throughout, to assist with understanding unfamiliar terms, explain concepts I didn't learn yet, and occasionally giving me a small nudge in the right direction if I was stuck on a single problem for too long









