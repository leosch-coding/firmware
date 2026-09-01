#include <stdint.h>
#include <stdbool.h>

// initializes all registers necessary for smooth operation in subsequent functions
void adc_init(void) {
    // toggles the clock for ADC1
    volatile uint32_t *RCC_APB2ENR = (uint32_t *)0x40023844;
    *RCC_APB2ENR |= 1 << 8;

    // disable VBATE as a prerequisite for temp.sense/V.REFINT being enabled
    // Enables temp.sensor and V.REFINT
    volatile uint32_t *ADC_CCR = (uint32_t *)0x40012304;
    *ADC_CCR &= ~(1 << 22);
    *ADC_CCR |= (1 << 23);


    // Sets the first conversion to 18, which is the channel for the internal temp. sensor
    volatile uint32_t *ADC_SQR3 = (uint32_t *)0x40012034;
    *ADC_SQR3 &= ~0x1F;
    *ADC_SQR3 |= 18;

    // Setting the sampling time to 480 cycles, as the temperature sensor requires 10 μs
    volatile uint32_t *ADC_SMPR1 = (uint32_t *)0x4001200C;
    *ADC_SMPR1 &= ~(0x7 << 24);
    *ADC_SMPR1 |= (0x7 << 24);


    // Setting the resolution to 12 bits, as its the most accurate and we have lots of wiggle room with project limits
    volatile uint32_t *ADC_CR1 = (uint32_t *)0x40012004;
    *ADC_CR1 &= ~(0x3 << 24);

    // Configures single conversion and turns on the ADC
    volatile uint32_t *ADC_CR2 = (uint32_t *)0x40012008;
    *ADC_CR2 &= ~(1 << 1);
    *ADC_CR2 |= (1 << 0);
}

void adc_start(void) {
}

bool adc_ready(void) {
}

uint16_t adc_read(void) {
}
