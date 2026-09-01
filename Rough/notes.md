# Objective 0: Getting the tool-chain set up

I just used the tools I used to start learning firmware that I installed two days ago. 
In theory, this should be enough, but if not I can add onto the toolchain.

Need to actually make it start and link first.
Need to learn what a vector table is before that though.

I now understand that a vector table is essentially a table of pointers to specific things that we start-up with. Like our reset_handler

We go: MCU turns on -> calls CPU -> consults vector table -> finds reset handler and etc -> reset handler does the hard boot stuff -> main can run (this is where our actual logic is)

We need to find the stack pointer first to set up main. Looking at the address table I can see that SRAM ends at 0x2001 FFFF. So it makes sense for why the stack would be 0x2002 0000.

I finished writing the vector table, now onto the linker

Linker is done, I'll just add in the reset handler and we can use default handlers for everything else right now.

I'm using nvim because I don't have time to learn another IDE

# Objective 1: Making a heartbeat

Now that we have our setup to start, we can start programming main

Need to read the datasheet/reference manual to find how to start a timer.

HSI is the initial 'source' for sysclk after a reset

It's clicked to me that to make our timer 1000 Hz, we need to derive a specific numerical value that will 'pass through' RCC and end up at 1000 Hz when it reaches the actual clock we're using.
In this case, I've selected TIM2, since it seems like the easiest due to it being at the 'lowest' possible address in APB1.

To refine this, I now also understand that we're not passing in some arbitrary value at all. We're just configuring things to divide/multiply/etc this clock value to turn it into 1000 Hz.

I could divide 16 MHz (the frequency of HSI which is our default counter for sysclk), but now that I realize I can just configure the TIM2 clock directly, I think that'd be easier.

The plan is to divide 16 MHz (in TIM2) to 1 MHz (/16)
Looking up some values, 1 MHz equals one microsecond.
1000 microseconds equals to one millisecond, and 1000 milliseconds is a second.
Our tick counter needs to get to 1000 before an interrupt happens. Interrupt should happen 1000 times, and we have the value we want.

Reading how TIM2 works, we need to set PSC to 15 (because from my knowledge PSC starts its 'count' at 0, which represents divide by 1), and ARR to 999 (same thing. It starts at index 0)
I'll need to figure out how to set the TIM2 registers to these values.

First we need to actually enable TIM2 by setting CC_APB1ENR bit 0, to 1.
Then we'll configure TIM2 registers; TIMx_PSC, TIMx_ARR, TIMx_DIER, and TIMx_CR1.

We know that RCC starts at 0x40023800, we add the offset for CC_APB1ENR (0x40), to get 0x40023840.

This is so much easier then asm

The files compile and link. Now to test the actual firmware.

That took...a while. I finally figured out that I was flipping the wrong bit.


## Objective 2: Setting up the ADC driver

I now know that I need to set up the peripheral's clock before I can use it.

Bit 8, 9, and 10 of RCC_APB2ENR are used to configure the clock for ADC.

Because we want temperature sensor data, we'll turn on the clock for ADC1.

It has only fully clicked now that I can use the shift operator. That would've been nice to know about 10 hours ago. 

Configuring the ADCCLK, we gotta derive it via the sysclk passing through the AHB prescalar and APB2 prescalar.

We know our initial sysclk is 16MHz, we're gonna leave the two prescalars alone for now.

This is mostly just busy work, aka, reading the reference manual/datasheet, finding registers, configuring registers, and repeat.



