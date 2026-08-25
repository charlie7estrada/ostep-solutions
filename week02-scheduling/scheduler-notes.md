# Homework (Simulation)
This program, scheduler.py, allows you to see how different schedulers perform under scheduling metrics such as response time, turnaround time, and total wait time. See the README for details.

# Questions
1. Compute the response time and turnaround time when running 
three jobs of length 200 with the SJF and FIFO schedulers
    `python3 scheduler.py -l 200,200,200 -p SJF -c`
    `python3 scheduler.py -l 200,200,200 -p FIFO -c`
    Result: Identical for both schedulers.
    Job 0 -- Response: 0, Turnaround: 200
    Job 1 -- Response: 200, Turnaround: 400
    Job 2 -- Response: 400, Turnaround: 600
    SJF and FIFO produce identical results here because SJF only matters when job lengths differ - with all jobs equal length, there's no meaningful "shortest job" to prioritize, so it just runs them in arrival order, same as FIFO.

2. Now do the same but with jobs of different lengths: 100, 200, and 300.
    `python3 scheduler.py -l 100,200,300 -p SJF -c`
    `python3 scheduler.py -l 100,200,300 -p FIFO -c`
    Result: Identical for both schedulers.
    Job 0 -- Response: 0, Turnaround: 100
    Job 1 -- Response: 100, Turnaround: 300
    Job 2 -- Response: 300, Turnaround: 600
    SJF and FIFO match here because the jobs happen to already be listed shortest-to-longest (100, 200, 300) - SJF's chosen order ends up being identical to FIFO's arrival order. If the jobs were listed in a different order (e.g. 300, 100, 200), SJF and FIFO would diverge

3. Now do the same, but also with the RR scheduler and a time-slice of 1.
    `python3 scheduler.py -l 100,200,300 -p RR -q 1 -c`
    Result:
    Job 0 -- Response: 0, Turnaround: 298
    Job 1 -- Response: 1, Turnaround: 499
    Job 2 -- Response: 2, Turnaround: 600

    RR with time-slice 1 dramatically improves response time (0,1,2 vs FIFO/SJF's 0,100,300) since every job gets some CPU almost immediately. But turnaround time gets significantly worse, especially for shorter jobs - Job 0 only needed 100 units of work but didn't finish until 298, since it keeps getting interrupted to let Jobs 1 and 2 take their turn. Job 2 (the longest job) finishes at the same time either way (600) since it was always going to be last regardless of scheduling policy
    This is the core tradeoff RR makes: fairness/responsiveness (good for interactive use) at the cost of overall completion time (bad if you just want jobs done as fast as possible). Neither RR nor FIFO/SJF is "better" universally - it depends what you're optimizing for.

4. For what types of workloads does SJF deliver the same turnaround times as FIFO?
    If all the processes are the same in length or in the same order, short first longest later

5. For what types of workloads and quantum lengths does SJF deliver
the same response times as RR?
    RR delivers the same response times as SJF when two conditions hold together: (1) the quantum is at least as large as the longest job, so RR never actually interrupts anyone mid-run and effectively behaves like FIFO, and (2) the jobs arrive already sorted shortest-to-longest, which is when FIFO matches SJF (from Q4). With both conditions true, RR collapses into FIFO, which collapses into SJF - so all three produce identical response times.

6. What happens to response time with SJF as job lengths increase?
Can you use the simulator to demonstrate the trend?
    `python3 scheduler.py -l 100,200,300 -p SJF -c` → Response: 0, 100, 300
    `python3 scheduler.py -l 1000,2000,3000 -p SJF -c` → Response: 0, 1000, 3000
    `python3 scheduler.py -l 10000,20000,30000 -p SJF -c` → Response: 0, 10000, 30000

    Response time scales linearly/proportionally with job length. Scaling all job lengths by 10x scaled every job's response time by exactly 10x. This makes sense because in SJF, a job's response time equals the sum of the lengths of all shorter jobs that run before it - since all lengths scaled together, that sum scaled by the same factor.
    This shows SJF's core weakness at scale: longer jobs get proportionally worse response times.

7. What happens to response time with RR as quantum lengths increase? Can you write an equation that gives the worst-case response time, given N jobs?
    `python3 scheduler.py -l 100,100,100,100 -p RR -q 10 -c`
    Result: Response times were 0, 10, 20, 30 for jobs 0-3 - each job's response time is exactly (its position in the rotation) × quantum.
    As quantum increases, response time increases proportionally for every job except the first. The worst-case response time (the last job in rotation) follows the equation:

    worst-case response time = (N - 1) × q

    where N = number of jobs, q = quantum length. This makes sense because the last job has to wait for all N-1 jobs ahead of it to each take one full quantum turn before it gets its own first turn. This confirms the earlier tradeoff (Q3): larger quantums push RR's response time closer to FIFO/SJF's, since fewer/longer interruptions mean less frequent turn-taking for jobs further back in line.