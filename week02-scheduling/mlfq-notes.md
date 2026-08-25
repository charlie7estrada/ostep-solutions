# Homework (Simulation)
This program, mlfq.py, allows you to see how the MLFQ scheduler
presented in this chapter behaves. See the README for details.

# Questions
1. Run a few randomly-generated problems with just two jobs and
two queues; compute the MLFQ execution trace for each. Make
your life easier by limiting the length of each job and turning off
I/Os.
    `python3 mlfq.py -n 2 -j 2 -m 100 -M 0 -c`

2. How would you run the scheduler to reproduce each of the examples in the chapter?
    Example 1: A Single Long-Running Job (fig 8.2) - shows basic demotion Q2→Q1→Q0
    `python3 mlfq.py -n 3 -q 10 -a 1 -l 0,200,0 -c`
    -n 3 — three queues, -q 10 — quantum of 10 for all queues, -a 1 — allotment of 1 (one time slice before demotion) -l 0,200,0 — one manually-defined job: starts at time 0, runs for 200, no I/O
    Example 2: Along Came A Short Job (fig 8.3 left) - short job B gets fast treatment by starting high priority
    `python3 mlfq.py -n 3 -q 10 -a 1 -l 0,200,0:100,20,0 -c`
    Example 3: What About I/O? (fig 8.3 right) - Rule 4b: job B stays high priority since it yields via I/O before using full quantum
    `python3 mlfq.py -n 3 -q 10 -a 1 -l 0,200,0:0,200,1 -c`

3. How would you configure the scheduler parameters to behave just
like a round-robin scheduler?
    `python3 mlfq.py -n 1 -q 10 -l 0,100,0:0,100,0 -c`
    With only one queue, there's nowhere for a job to be demoted to, so every job stays at PRIORITY 0 permanently. The scheduler just alternates between jobs every quantum (10 ticks), exactly like plain RR

4. Craft a workload with two jobs and scheduler parameters so that
one job takes advantage of the older Rules 4a and 4b (turned on
with the -S flag) to game the scheduler and obtain 99% of the CPU
over a particular time interval.
    `python3 mlfq.py -n 2 -q 10 -S -l 0,100,9:0,100,0 -c -S`
    -S turns on the old Rules 4a/4b, which allow gaming: a job that yields (via I/O) before its quantum expires stays at the same priority instead of being demoted.
    This confirms the core exploit mechanism: Job 0 successfully avoids demotion by yielding right at the edge of its quantum, while Job 1 gets punished for behaving normally.

5. Given a system with a quantum length of 10 ms in its highest queue,
how often would you have to boost jobs back to the highest priority
level (with the -B flag) in order to guarantee that a single longrunning (and potentially-starving) job gets at least 5% of the CPU?
    `python3 mlfq.py -n 3 -q 10 -B 200 -l 0,500,0:0,10,0 -c`
    long job + a small competing job, boost every 200ms - confirms starved job gets a full quantum each boost cycle

6. One question that arises in scheduling is which end of a queue to
add a job that just finished I/O; the -I flag changes this behavior
for this scheduling simulator. Play around with some workloads
and see if you can see the effect of this flag.
    -I controls whether a job returning from I/O goes to the FRONT (immediate, jumps ahead of jobs already waiting) or BACK (normal, waits its turn) of its queue.
    `python3 mlfq.py -n 2 -q 10 -M 5 -j 3 -c` (without -I)
    `python3 mlfq.py -n 2 -q 10 -M 5 -j 3 -I -c` (with -I)
    Effect: with -I, a job finishing I/O cuts to the front of the queue and runs almost immediately, giving I/O-bound jobs even faster turnaround. Without -I, it goes to the back and has to wait behind other ready jobs first, even though it's at the same priority level