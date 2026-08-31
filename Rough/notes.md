# Objective 0: Getting the tool-chain set up

I just used the tools I used to start learning firmware that I installed two days ago. 
In theory, this should be enough, but if not I can add onto the toolchain.

Need to actually make it start and link first.
Need to learn what a vector table is before that though.

Update: I know what a vector table is now.

We go: MCU turns on -> calls CPU -> consults vector table -> finds reset handler and etc -> reset handler does the hard boot stuff -> main can run (this is where our actual logic is)

We need to find the stack pointer first to set up main. Looking at the address table I can see that SRAM ends at 0x2001 FFFF. So it makes sense for why the stack would be 0x2002 0000.

I finished writing the vector table, now onto the linker

Linker is done, I'll just add in the reset handler and we can use default handlers for everything else right now.

I'm using nvim because I don't have time to learn another IDE

# Object 1: Making a heartbeat

Now that we have our setup to start, we can start programming main

Need to read the datasheet/reference manual to find how to start a timer.

HSI is the initial 'source' for sysclk after a reset
