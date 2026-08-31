void main(void) {
    // We make a pointer to APB1ENR
    volatile uint32_t *APB1ENR = (uint32_t *)0x40023840;
    // Use OR to set bit 0 to 1, turning TIM2 on
    *APB1ENR |= 0b1;
}
