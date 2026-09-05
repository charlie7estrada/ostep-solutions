# Homework (Simulation)
This simulator, paging-policy.py, allows you to play around with
different page-replacement policies. See the README for details.

# Questions
1. Generate random addresses with the following arguments: -s 0
-n 10, -s 1 -n 10, and -s 2 -n 10. Change the policy from
FIFO, to LRU, to OPT. Compute whether each access in said address
traces are hits or misses.
    `python3 paging-policy.py -s 0 -n 10 -p FIFO -c`
    `python3 paging-policy.py -s 0 -n 10 -p LRU -c`
    `python3 paging-policy.py -s 0 -n 10 -p OPT -c`
   
    FIFO: 1 hit, 9 misses - 10% hit rate
    LRU: 2 hits, 8 misses - 20% hit rate  
    OPT: 4 hits, 6 misses - 40% hit rate

    Result: Confirms the expected ranking exactly - OPT > LRU > FIFO, matching the chapter's theoretical claims.

2. For a cache of size 5, generate worst-case address reference streams
for each of the following policies: FIFO, LRU, and MRU (worst-case
reference streams cause the most misses possible. For the worst case
reference streams, how much bigger of a cache is needed to improve
performance dramatically and approach OPT?)
    FIFO worst case: looping-sequential pattern (0,1,2,3,4,5 repeated) - 6 unique pages cycling through a 5-page cache.
    `python3 paging-policy.py -a 0,1,2,3,4,5,0,1,2,3,4,5,0,1,2,3,4,5 -C 5 -p FIFO -c`
    Result: 0% hit rate (0 hits, 18 misses) - FIFO always evicts exactly the page about to loop back around.
    LRU worst case: same looping-sequential pattern also devastates LRU identically.
    `python3 paging-policy.py -a 0,1,2,3,4,5,0,1,2,3,4,5,0,1,2,3,4,5 -C 5 -p LRU -c`
    Result: 0% hit rate (0 hits, 18 misses) - identical eviction sequence to FIFO in this case, since "oldest inserted" and "least recently used" coincide perfectly in a clean N+1-page loop.
    UNOPT (worst-possible/anti-optimal policy) worst case: repeatedly reuse one popular page while cycling new pages through the rest of the cache.
    `python3 paging-policy.py -a 0,1,2,3,4,0,5,0,6,0,7,0,8,0 -C 5 -p UNOPT -c`
    Result: 7.14% hit rate (1 hit, 13 misses) - UNOPT specifically evicts whatever will be needed soonest (the opposite of OPT), so it punishes the most popular page (0) every time, evicting it right before each reuse.

    How much bigger a cache is needed to approach OPT: for the looping-sequential case specifically, the cache just needs to be >= the number of unique pages in the loop (6, in this example) - once the whole working set fits, EVERY policy (even the worst ones) trivially hits 100%, since nothing ever needs to be evicted at all. This matches the chapter's own graph (Fig 22.8) showing all policies converging once cache size >= working set size.

3. Generate a random trace (i.e., use python and write a script that
outputs random addresses, which you can then feed into the simulator). How would you expect the different policies to perform on
such a trace?
    Expected behavior: with a purely random reference stream (no locality at all), FIFO, LRU, and RAND should all perform roughly the same - hit rate determined almost entirely by cache size relative to the number of unique pages, since there's no meaningful pattern for any "smart" policy to exploit.


4. Now generate a trace with some locality. How can you generate
such a trace? How does LRU perform on it? How much better than
RAND is LRU? How does CLOCK do? How about CLOCK with
different numbers of clock bits?
    How to generate one: instead of pure uniform randomness, bias page selection so some pages are accessed far more often than others - e.g. an "80-20" pattern (80% of accesses go to just 20% of pages), similar to the book's own 80-20 Workload example.
    Expected results: LRU should meaningfully outperform RAND here (unlike Q3), since LRU can actually exploit the fact that "hot" pages get reused often - keeping frequently-accessed pages in cache pays off. CLOCK (LRU's cheap hardware-friendly approximation) should land between LRU and RAND - capturing most of LRU's benefit without the full bookkeeping cost. More clock bits (multiple reference bits instead of just one) generally let CLOCK approximate LRU more closely, since it can distinguish more gradations of "how recently used" rather than just a single yes/no bit - closer approximation, closer performance to real LRU.

5. Use a program like valgrind to instrument a real application and
generate a virtual page reference stream. For example, running
valgrind --tool=lackey --trace-mem=yes ls will output
a nearly-complete reference trace of every instruction and data reference made by the program ls. To make this useful for the simulator above, you’ll have to first transform each virtual memory
reference into a virtual page-number reference (done by masking
off the offset and shifting the resulting bits downward). How big
of a cache is needed for your application trace in order to satisfy a
large fraction of requests? Plot a graph of its working set as the size
of the cache increases.
    Using `valgrind --tool=lackey --trace-mem=yes <program>` captures a real reference stream from an actual running program, rather than a synthetic/assumed pattern - this is the most authentic test of a replacement policy, since real programs have their own genuine (and often uneven) locality patterns baked into how they actually use memory. The expected shape of the result: plotting hit rate against cache size should reveal the program's actual "working set" size - the point where hit rate sharply climbs toward 100%, showing roughly how many pages the program is actively cycling through at once. Below that size, performance suffers regardless of policy; above it, most policies converge and do reasonably well - directly connecting back to Ch22's thrashing discussion (Section 22.11) - if available cache/memory sits below a program's actual working set, no policy can save you from constant misses.