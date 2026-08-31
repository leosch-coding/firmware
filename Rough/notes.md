# Objective 0: Getting the tool-chain set up

I just used the tools I used to start learning firmware that I installed two days ago. 
In theory, this should be enough, but if not I can add onto the toolchain.

I'm using nvim because I don't have time to learn another IDE

# Object 1: Making a heartbeat

I need to find what addresses in memory are mapped to timer config, and then configure it correctly in order.
After of which I'll need to get the LED2 working. They gave me PA5 thankfully, so I won't have to manually look that up.

My current mental model goes:

Do something with RCC to get the clock setup and all

Configure TIM

Configure LED2?

Use a GPIO to get them all to talk to eachother???

I'll update my mental model as I go.
