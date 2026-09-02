.syntax unified
.cpu cortex-m4
.thumb

.section .isr_vector, "a", %progbits

.word 0x20020000
.word Reset_Handler
.word Default_Handler @ NMI
.word Default_Handler @ HardFault
.word Default_Handler @ MemManage
.word Default_Handler @ Busfault
.word Default_Handler @ UsageFault
.word 0x00000000
.word 0x00000000
.word 0x00000000
.word 0x00000000
.word Default_Handler @ SVCall
.word Default_Handler @ Debug_Montior
.word 0x00000000
.word Default_Handler @ PendSV
.word Default_Handler @ Systick
.word Default_Handler @ WWDG
.word Default_Handler @ PVD
.word Default_Handler @ TAMP_STAMP
.word Default_Handler @ RTC_WKUP
.word Default_Handler @ FLASH
.word Default_Handler @ RCC
.word Default_Handler @ EXTI0
.word Default_Handler @ EXTI1
.word Default_Handler @ EXTI2
.word Default_Handler @ EXTI3
.word Default_Handler @ EXTI4
.word Default_Handler @ DMA1_Stream0
.word Default_Handler @ DMA1_Stream1
.word Default_Handler @ DMA1_Stream2
.word Default_Handler @ DMA1_Stream3
.word Default_Handler @ DMA1_Stream4
.word Default_Handler @ DMA1_Stream5
.word Default_Handler @ DMA1_Stream6
.word Default_Handler @ ADC
.word Default_Handler @ CAN1_TX
.word Default_Handler @ CAN1_RX0
.word Default_Handler @ CAN1_RX1
.word Default_Handler @ CAN1_SCE
.word Default_Handler @ EXTI9_5
.word Default_Handler @ TIM1_BRK_TIM9 
.word Default_Handler @ TIM1_UP_TIM10
.word Default_Handler @ TIM1_TRG_COM_TIM11
.word Default_Handler @ TIM1_CC
.word TIM2_Interrupt_Handler @ TIM2
.word Default_Handler @ TIM3
.word Default_Handler @ TIM4
.word Default_Handler @ I2C1_EV
.word Default_Handler @ I2C1_ER
.word Default_Handler @ I2C2_EV
.word Default_Handler @ I2C2_ER
.word Default_Handler @ SPI1
.word Default_Handler @ SPI2
.word Default_Handler @ USART1
.word Default_Handler @ USART2
.word Default_Handler @ USART3
.word Default_Handler @ EXTI15_10
.word Default_Handler @ RTC_Alarm 
.word Default_Handler @ OTG_FS_WKUP
.word Default_Handler @ TIM8_BRK_TIM12
.word Default_Handler @ TIM8_UP_TIM13 
.word Default_Handler @ TIM8_TRG_COM_TIM14
.word Default_Handler @ TIM8_CC
.word Default_Handler @ DMA1_Stream7
.word Default_Handler @ FMC
.word Default_Handler @ SDIO 
.word Default_Handler @ TIM5
.word Default_Handler @ SPI3
.word Default_Handler @ UART4
.word Default_Handler @ UART5
.word Default_Handler @ TIM6_DAC
.word Default_Handler @ TIM7
.word Default_Handler @ DMA2_Stream0
.word Default_Handler @ DMA2_Stream1
.word Default_Handler @ DMA2_Stream2
.word Default_Handler @ DMA2_Stream3
.word Default_Handler @ DMA2_Stream4
.word 0x00000000
.word 0x00000000
.word Default_Handler @ CAN2_TX
.word Default_Handler @ CAN2_RX0
.word Default_Handler @ CAN2_RX1
.word Default_Handler @ CAN2_SCE
.word Default_Handler @ OTG_FS
.word Default_Handler @ DMA2_Stream5
.word Default_Handler @ DMA2_Stream6
.word Default_Handler @ DMA2_Stream7
.word Default_Handler @ USART6
.word Default_Handler @ I2C3_EV
.word Default_Handler @ I2C3_ER
.word Default_Handler @ OTG_HS_EP1_OUT
.word Default_Handler @ OTG_HS_EP1_IN
.word Default_Handler @ OTG_HS_WKUP
.word Default_Handler @ OTG_HS
.word Default_Handler @ DCMI
.word 0x00000000
.word 0x00000000
.word Default_Handler @ FPU
.word 0x00000000
.word 0x00000000
.word Default_Handler @ SPI4
.word 0x00000000
.word 0x00000000
.word Default_Handler @ SAI1
.word 0x00000000
.word 0x00000000
.word 0x00000000
.word Default_Handler @ SAI2
.word Default_Handler @ QuadSPI
.word Default_Handler @ HDMI_CEC
.word Default_Handler @ SPDIF_Rx
.word Default_Handler @ FMPI2C1
.word Default_Handler @ FMPI2C1_error



.section .text
.global Reset_Handler
.type Reset_Handler, %function

.extern main

Reset_Handler:
    ldr r0, =_data_end
    ldr r1, =_data_load
    ldr r2, =_data_start

.loop_data:
    ldr r12, [r1] 

    str r12, [r2]

    add r1, r1, #4 
    add r2, r2, #4 

    cmp r0, r2
    beq .init_bss

    b .loop_data

.init_bss:
    ldr r0, =_bss_start
    ldr r1, =_bss_end
    mov r2, #0

.loop_bss:
    str r2, [r0]
    add r0, r0, #4

    cmp r0, r1
    beq .end

    b .loop_bss

.end:
    b main


.global Default_Handler
.type Default_Handler, %function

Default_Handler:
    b .


.global TIM2_Interrupt_Handler
.type TIM2_Interrupt_Handler, %function

.extern counter

TIM2_Interrupt_Handler:
    push {r4, lr}
    bl counter
    pop {r4, lr}
    bx lr
