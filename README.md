# Firmware Project

This is a repository for me to practice embedded/firmware programming

Languages used: C

Board used: NUCLEO-F446RE

Compiler: arm-none-eabi-gcc

Linker: arm-none-eabi-ld

Debugger/How I flashed it: arm-none-eabi-gdb

Connection to the board: OpenOCD

IDE: nvim

## Commands used (in order):

**To Compile**:

arm-none-eabi-gcc -c main.c -mcpu=cortex-m4 -mthumb -o main.o
arm-none-eabi-gcc -c adc.c -mcpu=cortex-m4 -mthumb -o adc.o
arm-none-eabi-gcc -c startup.s -mcpu=cortex-m4 -mthumb -o startup.o

**To Link**:

arm-none-eabi-gcc main.o adc.o startup.o -mcpu=cortex-m4 -mthumb -T linker.ld -nostdlib -o main.elf


**To flash**:

openocd -f interface/stlink.cfg -f target/stm32f4x.cfg
arm-none-eabi-gdb main.elf

(In gdb)
target extended-remote localhost:3333
monitor reset halt
load
monitor reset
continue

## Extra Information

Interrupt pin used: PB5

How to open serial port: I'll figure it out eventually

Startup and linker file is found in `/startup`
