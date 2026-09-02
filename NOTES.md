1.

2.

3.

4. I measured irs_us by using two global variables, initial_cycle_count and final_cycle_count. One taken at the beginning of function `count`, and one at the end.
   In `main`, I subtract final_cycle_count by initial_cycle_count to find elapsed_cycles.
   As our CPU cycles at 16MHz, 16 cycles = 1 μs 

6. I did not reach requirement 7. If I were to try and force it to break, I would attempt using the real time clock to stall each time by enough to cause it to miss its deadline.
   If that did not work, and I needed to brute-force it, I would probably attempt next using a while-loop that counts to an absurdly high number to stall until it misses its deadline


7. I was not able to get it to output anything in time.
   To avoid overwhelming myself, I tackled one problem at a time.
   To avoid getting caught up on overengineering each step, I planned to build 3, 4, and 5 sequentially and smoothen out logical bugs after it compiled.
   Both of these strategies backfired when I realized that I could not use printf to easily see output. I did not have enough time to learn how UART learned from scratch like I could with the rest of the functionality.
   I used AI to generate now-commented out code to see what the output would've been.
   The outputs were mostly correct, but my tick counter was off. This is something I'm researching deeper into.
   

8. Throughout, to assist with understanding unfamiliar terms, explain concepts I didn't learn yet, and occasionally giving me a small nudge in the right direction if I was stuck on a single problem for too long









