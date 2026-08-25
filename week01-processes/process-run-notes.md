# Homework (Simulation)
This program, process-run.py, allows you to see how process states
change as programs run and either use the CPU (e.g., perform an add
instruction) or do I/O (e.g., send a request to a disk and wait for it to
complete). See the README for details.

# Questions
1. Run process-run.py with the following flags: -l 5:100,5:100.
What should the CPU utilization be (e.g., the percent of time the
CPU is in use?) Why do you know this? Use the -c and -p flags to
see if you were right
        The cpu utilization should be 100% the entire time because there is no I/O happening, meaning the cpu is never waiting or blocked.
        `python3 process-run.py -l 5:100,5:100 -c -p`
        Result: Confirmed - CPU Busy 10 (100.00%). Process 0 runs all 5 instructions to completion, then Process 1 runs - no interleaving since there's no I/O to trigger a switch.

2. Now run with these flags: ./process-run.py -l 4:100,1:0.
These flags specify one process with 4 instructions (all to use the
CPU), and one that simply issues an I/O and waits for it to be done.
How long does it take to complete both processes? Use -c and -p
to find out if you were right
        Im guessing it would take 5 units of time, 4 to run process 0 and 1 to run process 1
        `python3 process-run.py -l 4:100,1:0 -c -p`
        Result: Wrong - total time = 11. Process 0 runs 4 instructions which takes 4 units, then process 1's I/O (issue, wait, and completion) takes 7 units, bringing the total to 11 units of time.

3. Switch the order of the processes: -l 1:0,4:100. What happens
now? Does switching the order matter? Why? (As always, use -c
and -p to see if you were right)
        It should take about 7 units of time, as the cpu can run the process 1's 4 instructions while the process 0's I/O is blocked.
        `python3 process-run.py -l 1:0,4:100 -c -p`
        Result: Confirmed - total time = 7 because Process 1's CPU work fills the time Process 0 spends blocked on I/O, instead of the CPU sitting idle.

4. We’ll now explore some of the other flags. One important flag is
-S, which determines how the system reacts when a process issues an I/O. With the flag set to SWITCH ON END, 
the system will NOT switch to another process while one is doing I/O, instead waiting until the process is completely finished. 
What happens when you run the following two processes (-l 1:0,4:100 -c -S SWITCH ON END), one doing I/O and the other doing CPU work?
        It should take 11 units of time since we have to wait for the I/O to be complete before running the CPU work.
        `python3 process-run.py -l 1:0,4:100 -c -S SWITCH_ON_END -p`
        Result: Confirmed - total time = 11. Using -S SWITCH_ON_END effectively defeats the purpose of reordering as highlighted in Q3.

5. Now, run the same processes, but with the switching behavior set
to switch to another process whenever one is WAITING for I/O (-l
1:0,4:100 -c -S SWITCH ON IO). What happens now? Use -c
and -p to confirm that you are right.
        This should be back to 7 units of time, since the scheduler now shifts the workload when the I/O is blocked. Which is the default behavior we saw in Q3 isnt it?
        `python3 process-run.py -l 1:0,4:100 -c -S SWITCH_ON_IO -p`
        Result: Confirmed - 7 units. Same exact result as Q3.

6. One other important behavior is what to do when an I/O completes. With -I IO RUN LATER, when an I/O completes, the process that issued it is not necessarily run right away; rather, whatever was running at the time keeps running. What happens when you run this combination of processes? (./process-run.py -l
3:0,5:100,5:100,5:100 -S SWITCH ON IO -c -p -I IO RUN LATER) Are system resources being effectively utilized?
        Process 0's I/O will most likely be delayed.
        `python3 process-run.py -l 3:0,5:100,5:100,5:100 -S SWITCH_ON_IO -c -p -I IO_RUN_LATER`
        Result: Confirmed, process 0 was delayed until the other 3 CPU processes completed. This is ineffecient, since the I/O consisted of 3 instructions the second and third rounds caused the CPU to be idle unneccessarily.

7. Now run the same processes, but with -I IO RUN IMMEDIATE set,
which immediately runs the process that issued the I/O. How does
this behavior differ? Why might running a process that just completed an I/O again be a good idea?
        This should be more efficient. Sounds like we can run the CPU processes while the I/O is blocked, then complete it and start the next I/O and so on.
        `python3 process-run.py -l 3:0,5:100,5:100,5:100 -S SWITCH_ON_IO -c -p -I IO_RUN_IMMEDIATE`
        Result: Much more efficient. CPU Usage at 100%, and took 21 units of time compared to 31 at Q6. This method gives the CPU more opportunities to overlap other work with it, instead of stalling like we saw in Q6.

8. Now run with some randomly generated processes using flags -s
1 -l 3:50,3:50 or -s 2 -l 3:50,3:50 or -s 3 -l 3:50,
3:50. See if you can predict how the trace will turn out. What happens when you use the flag -I IO RUN IMMEDIATE versus that
flag -I IO RUN LATER? What happens when you use the flag -S
SWITCH ON IO versus -S SWITCH ON END?
        `python3 process-run.py -s 1 -l 3:50,3:50 -I IO_RUN_IMMEDIATE -S SWITCH_ON_IO -c -p` TOTAL: 15 CPU: 53%
        `python3 process-run.py -s 1 -l 3:50,3:50 -I IO_RUN_LATER -S SWITCH_ON_IO -c -p` TOTAL: 15 CPU: 53%
        `python3 process-run.py -s 1 -l 3:50,3:50 -I IO_RUN_IMMEDIATE -S SWITCH_ON_END -c -p` TOTAL: 18 CPU: 44%
        `python3 process-run.py -s 1 -l 3:50,3:50 -I IO_RUN_LATER -S SWITCH_ON_END -c -p` TOTAL: 18 CPU: 44%
        -I IO RUN IMMEDIATE is generally more efficient. In the event of back to back I/Os in a particular process running them immediately will save time in the long run because we can still run CPU processes in the mean time. versus the flag -I IO RUN LATER generally takes more time for the opposite reason, we have to wait for all the CPU processes to complete then run the I/O, only to get blocked on the next I/O. 
        SWITCH_ON_END is worse - the CPU won't hand off to a READY process even when the running one blocks on I/O, wasting time it doesn't have to.

        **IO_RUN_IMMEDIATE only matters when there's contention - another process actively running when the I/O completes. With this seed, Process 1 was always either done or not yet started, so the flag had nothing to override**