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

# Object 1: Making a heartbeat

Now that we have our setup to start, we can start programming main

Need to read the datasheet/reference manual to find how to start a timer.

HSI is the initial 'source' for sysclk after a reset

It's clicked to me that to make our timer 1000 Hz, we need to derive a specific numerical value that will 'pass through' RCC and end up at 1000 Hz when it reaches the actual clock we're using.
In this case, I've selected TIM2, since it seems like the easiest due to it being at the 'lowest' possible address in APB1.

To refine this, I now also understand that we're not passing in some arbitrary value at all. We're just configuring things to divide/multiply/etc this clock value to turn it into 1000 Hz.

