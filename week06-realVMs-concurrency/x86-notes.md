# Homework (Simulation)
This program, x86.py, allows you to see how different thread interleavings either cause or avoid race conditions. See the README for details on how the program works, then answer the questions below.

# Questions
1. Let’s examine a simple program, “loop.s”. First, just read and understand it. Then, run it with these arguments (./x86.py -t 1
-p loop.s -i 100 -R dx) This specifies a single thread, an interrupt every 100 instructions, and tracing of register %dx. What
will %dx be during the run? Use the -c flag to check your answers;
the answers, on the left, show the value of the register (or memory
value) after the instruction on the right has run.
    `python3 ./x86.py -t 1 -p loop.s -i 100 -R dx -c`
    %dx starts at 0 by default (no -a flag set it). After `sub $1,%dx`,
    dx = -1. `jgt .top` only jumps if dx > 0, so it falls through to halt
    instead of looping. Loop body only executes once.
    Results:
    dx          Thread 0         

    0   
   -1   1000 sub  $1,%dx
   -1   1001 test $0,%dx
   -1   1002 jgt .top
   -1   1003 halt

2. Same code, different flags: (./x86.py -p loop.s -t 2 -i 100
-a dx=3,dx=3 -R dx) This specifies two threads, and initializes
each %dx to 3. What values will %dx see? Run with -c to check.
Does the presence of multiple threads affect your calculations? Is
there a race in this code?
    `python3 ./x86.py -p loop.s -t 2 -i 100 -a dx=3,dx=3 -R dx -c`
    Two threads, each with dx initialized to 3 (separate/private registers).
    Interrupt frequency (100) never triggers since each thread only takes ~13
    instructions to finish, so there's no interleaving, just Thread 0 running
    to completion (halt), then a Halt;Switch to Thread 1 which runs the same
    countdown independently. No race, because dx is per-thread, not shared.

    Results:
    dx          Thread 0                Thread 1         

    3   
    2   1000 sub  $1,%dx
    2   1001 test $0,%dx
    2   1002 jgt .top
    1   1000 sub  $1,%dx
    1   1001 test $0,%dx
    1   1002 jgt .top
    0   1000 sub  $1,%dx
    0   1001 test $0,%dx
    0   1002 jgt .top
    0   1003 halt
    3   ----- Halt;Switch -----  ----- Halt;Switch -----  
    2                            1000 sub  $1,%dx
    2                            1001 test $0,%dx
    2                            1002 jgt .top
    1                            1000 sub  $1,%dx
    1                            1001 test $0,%dx
    1                            1002 jgt .top
    0                            1000 sub  $1,%dx
    0                            1001 test $0,%dx
    0                            1002 jgt .top
    0                            1003 halt


3. Run this: ./x86.py -p loop.s -t 2 -i 3 -r -R dx -a
dx=3,dx=3 This makes the interrupt interval small/random; use
different seeds (-s) to see different interleavings. Does the interrupt frequency change anything?
    skipping for time

4. Now, a different program, looping-race-nolock.s, which accesses a shared variable located at address 2000; we’ll call this variable value. Run it with a single thread to confirm your understanding: ./x86.py -p looping-race-nolock.s -t 1 -M
2000 What is value (i.e., at memory address 2000) throughout the
run? Use -c to check.
    `python3 ./x86.py -p looping-race-nolock.s -t 1 -M 2000 -c`
    Single thread, value at address 2000 starts at 0 (memory defaults to 0,
    like registers). bx also defaults to 0 (no -a flag set it), same trap
    as Q1, but this program's structure is different: it does the critical
    section (mov/add/mov) FIRST, then checks the loop counter after. So the increment always runs at least once regardless of bx's starting value. Final value = 1.

    Results:
     2000          Thread 0         
    0   
    0   1000 mov 2000, %ax
    0   1001 add $1, %ax
    1   1002 mov %ax, 2000
    1   1003 sub  $1, %bx
    1   1004 test $0, %bx
    1   1005 jgt .top
    1   1006 halt

5. Run with multiple iterations/threads: ./x86.py -p
looping-race-nolock.s -t 2 -a bx=3 -M 2000 Why does
each thread loop three times? What is final value of value?
    `python3 ./x86.py -p looping-race-nolock.s -t 2 -a bx=3 -M 2000 -c`
    Two threads, both bx=3 (single -a value broadcasts to both threads).
    bx is a loop counter: body runs once per pass regardless, then sub
    decrements bx and jgt only loops back if bx is still >0. Starting at
    3 means exactly 3 passes before bx hits 0 and falls through to halt.
    Default interrupt frequency (50) is way higher than each thread's
    total instruction count (~19), so no interleaving happens. Thread 0
    runs all 3 iterations to completion (value 0->3), halts, switches to
    Thread 1, which picks up from the shared value and adds 3 more
    (3->6). Final value = 6, no race since threads never overlap.

    Results:
     2000          Thread 0                Thread 1         
    0   
    0   1000 mov 2000, %ax
    0   1001 add $1, %ax
    1   1002 mov %ax, 2000
    1   1003 sub  $1, %bx
    1   1004 test $0, %bx
    1   1005 jgt .top
    1   1000 mov 2000, %ax
    1   1001 add $1, %ax
    2   1002 mov %ax, 2000
    2   1003 sub  $1, %bx
    2   1004 test $0, %bx
    2   1005 jgt .top
    2   1000 mov 2000, %ax
    2   1001 add $1, %ax
    3   1002 mov %ax, 2000
    3   1003 sub  $1, %bx
    3   1004 test $0, %bx
    3   1005 jgt .top
    3   1006 halt
    3   ----- Halt;Switch -----  ----- Halt;Switch -----  
    3                            1000 mov 2000, %ax
    3                            1001 add $1, %ax
    4                            1002 mov %ax, 2000
    4                            1003 sub  $1, %bx
    4                            1004 test $0, %bx
    4                            1005 jgt .top
    4                            1000 mov 2000, %ax
    4                            1001 add $1, %ax
    5                            1002 mov %ax, 2000
    5                            1003 sub  $1, %bx
    5                            1004 test $0, %bx
    5                            1005 jgt .top
    5                            1000 mov 2000, %ax
    5                            1001 add $1, %ax
    6                            1002 mov %ax, 2000
    6                            1003 sub  $1, %bx
    6                            1004 test $0, %bx
    6                            1005 jgt .top
    6                            1006 halt

6. Run with random interrupt intervals: ./x86.py -p
looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 0 with
different seeds (-s 1, -s 2, etc.) Can you tell by looking at the
thread interleaving what the final value of value will be? Does the
timing of the interrupt matter? Where can it safely occur? Where
not? In other words, where is the critical section exactly?
    `python3 ./x86.py -p looping-race-nolock.s -t 2 -M 2000 -i 4 -r -s 1 -c`
    Random interrupts every ~1-4 instructions. This seed lands an interrupt
    right after Thread 0 loads value (0) into its %ax but before it can
    add/store. Thread 1 then runs its full critical section uninterrupted
    (0->1). When Thread 0 resumes, it's still holding its stale %ax=0,
    increments to 1, and overwrites address 2000 with 1 again. Thread 1's
    increment gets silently lost even though it "happened."
    Final value = 1 (expected 2 if run sequentially). This is the lost
    update. Critical section = mov 2000,%ax through mov %ax,2000; any
    interrupt landing inside that window is unsafe.

    Results:
     2000          Thread 0                Thread 1         
    0   
    0   1000 mov 2000, %ax
    0   ------ Interrupt ------  ------ Interrupt ------  
    0                            1000 mov 2000, %ax
    0                            1001 add $1, %ax
    1                            1002 mov %ax, 2000
    1                            1003 sub  $1, %bx
    1   ------ Interrupt ------  ------ Interrupt ------  
    1   1001 add $1, %ax
    1   1002 mov %ax, 2000
    1   1003 sub  $1, %bx
    1   1004 test $0, %bx
    1   ------ Interrupt ------  ------ Interrupt ------  
    1                            1004 test $0, %bx
    1                            1005 jgt .top
    1   ------ Interrupt ------  ------ Interrupt ------  
    1   1005 jgt .top
    1   1006 halt
    1   ----- Halt;Switch -----  ----- Halt;Switch -----  
    1   ------ Interrupt ------  ------ Interrupt ------  
    1                            1006 halt

7. Now examine fixed interrupt intervals: ./x86.py -p
looping-race-nolock.s -a bx=1 -t 2 -M 2000 -i 1 What
will the final value of the shared variable value be? What about
when you change -i 2, -i 3, etc.? For which interrupt intervals
does the program give the “correct” answer?
    skipping for time

8. Run the same for more loops (e.g., set -a bx=100). What interrupt intervals (-i) lead to a correct outcome? Which intervals are
surprising?
    skipping for time

9. One last program: wait-for-me.s. Run: ./x86.py -p
wait-for-me.s -a ax=1,ax=0 -R ax -M 2000 This sets the
%ax register to 1 for thread 0, and 0 for thread 1, and watches %ax
and memory location 2000. How should the code behave? How is
the value at location 2000 being used by the threads? What will its
final value be?
    `python3 ./x86.py -p wait-for-me.s -a ax=1,ax=0 -R ax -M 2000 -c`
    Thread 0 gets ax=1 (signaller), Thread 1 gets ax=0 (waiter). Shared
    flag at address 2000 starts at 0. Default interrupt frequency (50)
    means both threads run uninterrupted in program order: Thread 0 runs
    to completion first, sets value=1, halts. Thread 1 doesn't start until
    after, so when it checks the flag it's already set, jne doesn't loop,
    goes straight to halt. Waiter never actually spins in this run, the
    wait-for-flag mechanism only shows its looping behavior when the
    waiter runs BEFORE the signaller sets the flag.

    Results:
     2000      ax          Thread 0                Thread 1         
    0       1   
    0       1   1000 test $1, %ax
    0       1   1001 je .signaller
    1       1   1006 mov  $1, 2000
    1       1   1007 halt
    1       0   ----- Halt;Switch -----  ----- Halt;Switch -----  
    1       0                            1000 test $1, %ax
    1       0                            1001 je .signaller
    1       0                            1002 mov  2000, %cx
    1       0                            1003 test $1, %cx
    1       0                            1004 jne .waiter
    1       0                            1005 halt

10. Now switch the inputs: ./x86.py -p wait-for-me.s -a
ax=0,ax=1 -R ax -M 2000 How do the threads behave? What
is thread 0 doing? How would changing the interrupt interval (e.g.,
-i 1000, or perhaps to use random intervals) change the trace outcome? Is the program efficiently using the CPU?
    `python3 ./x86.py -p wait-for-me.s -a ax=0,ax=1 -R ax -M 2000 -c`
    Thread 0 = waiter (ax=0), Thread 1 = signaller (ax=1). Thread 0 runs
    first (default schedule) and immediately starts spinning: checks flag
    at 2000, finds it 0, jumps back, repeats ~16 times. It's not blocked,
    it's actively burning CPU doing nothing, this is busy-waiting.
    Thread 1 can't run and set the flag until the timer interrupt (default
    freq 50) forcibly preempts Thread 0 mid-spin. Once switched, Thread 1
    runs the signaller uninterrupted, sets value=1, halts. Switch back to
    Thread 0, which now sees the flag set, exits the loop, halts.
    Inefficient CPU use: the waiter wastes cycles until the scheduler
    happens to preempt it. A longer interrupt interval (-i 1000) would
    mean even more wasted spinning before Thread 1 gets a turn.

    Results: (truncated repeating spin block, ~16 iterations)
     2000      ax          Thread 0                Thread 1         
    0       0   
    0       0   1000 test $1, %ax
    0       0   1001 je .signaller
    0       0   1002 mov  2000, %cx
    0       0   1003 test $1, %cx
    0       0   1004 jne .waiter
    ... (repeats mov/test/jne ~16x) ...
    0       1   ------ Interrupt ------  ------ Interrupt ------  
    0       1                            1000 test $1, %ax
    0       1                            1001 je .signaller
    1       1                            1006 mov  $1, 2000
    1       1                            1007 halt
    1       0   ----- Halt;Switch -----  ----- Halt;Switch -----  
    1       0   1002 mov  2000, %cx
    1       0   1003 test $1, %cx
    1       0   1004 jne .waiter
    1       0   1005 halt