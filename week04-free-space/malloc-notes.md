# Homework (Simulation)
The program, malloc.py, lets you explore the behavior of a simple
free-space allocator as described in the chapter. See the README for
details of its basic operation.

# Questions
1. First run with the flags -n 10 -H 0 -p BEST -s 0 to generate a few random allocations and frees. Can you predict what alloc()/free() will return? Can you guess the state of the free list after
each request? What do you notice about the free list over time?
    `python3 malloc.py -n 10 -H 0 -p BEST -s 0`
    Heap: base=1000, size=100, no headers, no coalescing.

    Traced by hand through all 10 ops (allocs split the smallest sufficient chunk; frees just re-add as a separate entry, no merging since -C not set).
    Final free list: [1002,1] [1003,5] [1015,1] [1016,84]

    Key observations:
    - Best fit's weakness: 3 of 4 final chunks are tiny slivers (1, 5, 1 bytes) - essentially wasted/unusable, while one big 84-byte chunk absorbs everything meaningful.
    - "searched N elements" grows every operation (1,2,3,4...) - confirms best fit's real cost: it scans the entire free list every time.
    - No coalescing means freed chunks never merge back together, even when adjacent - this compounds the fragmentation problem directly.

2. How are the results different when using a WORST fit policy to
search the free list (-p WORST)? What changes?
    `python3 malloc.py -n 10 -H 0 -p WORST -s 0 -c`
    Result: WORST fit performed worse than BEST fit on both metrics:
    - 5 free chunks at the end (vs BEST's 4) - more fragmentation
    - Largest remaining chunk shrank to 67 bytes (vs BEST's 84) - the opposite of worst fit's own goal

    Divergence point: step 4 (2nd Alloc(8)). BEST correctly reused an exact-fit 8-byte leftover chunk from a previous free (no waste). WORST ignored that perfect match and split the largest chunk (84 bytes) instead, just because its rule is "always take the biggest" - actively working against efficient reuse.

    Worst fit sounds reasonable in theory ("leave one big chunk instead of many small ones") but performs badly in practice - it's not just slow, it also fragments more.

3. What about when using FIRST fit (-p FIRST)? What speeds up
when you use first fit?
    `python3 malloc.py -n 10 -H 0 -p FIRST -s 0 -c`
    Result: Final free list identical to BEST fit: [1002,1] [1003,5] [1015,1] [1016,84]
    But search counts were consistently lower or equal at every step (e.g. step 4: FIRST searched 3 vs BEST/WORST's 4; step 5: FIRST searched just 1).

    What speeds up: FIRST fit avoids scanning the entire free list every time - it stops the moment it finds any chunk that's big enough, rather than continuing to check if a "better" (smaller/larger) one exists elsewhere. This makes it faster than both BEST and WORST fit, which always require a full list scan.

4. For the above questions, how the list is kept ordered can affect the
time it takes to find a free location for some of the policies. Use
the different free list orderings (-l ADDRSORT, -l SIZESORT+,
-l SIZESORT-) to see how the policies and the list orderings interact.
    ADDRSORT (default): doesn't speed up BEST/WORST (still full scans required), but keeps FIRST fit from clustering picks at the start of memory, and makes coalescing easy since adjacent memory = adjacent list entries.

    SIZESORT+ (smallest first): BEST fit becomes fast (first sufficient match = smallest match, by construction). WORST fit becomes SLOWER (largest chunk is now at the back, forces full scan). FIRST fit effectively becomes BEST fit's behavior.

    SIZESORT- (largest first): mirror image - WORST fit becomes fast, BEST fit becomes slow, FIRST fit effectively becomes WORST fit's behavior.

    Key takeaway: FIRST fit's name is a bit misleading - "first" only means something once you know the list order. FIRST fit isn't really its own independent strategy; it's a speed optimization that inherits whatever behavior the underlying list order encodes.

5. Coalescing of a free list can be quite important. Increase the number
of random allocations (say to -n 1000). What happens to larger
allocation requests over time? Run with and without coalescing
(i.e., without and with the -C flag). What differences in outcome do
you see? How big is the free list over time in each case? Does the
ordering of the list matter in this case?
    `python3 malloc.py -n 1000 -H 0 -p BEST -s 0 -c`
    `python3 malloc.py -n 1000 -H 0 -p BEST -s 0 -C -c`

    Without -C: final free list = 31 chunks, ALL sized 1-6 bytes. Heap totally fragmented - technically has free space, but nothing usable for any request bigger than ~6 bytes.

    With -C: final free list = 1 chunk, size 98 - almost the entire heap restored to one usable block.

    What happens to larger requests over time: without coalescing, larger allocations increasingly fail as the run progresses, since the free list degrades into tiny unusable slivers

    Does list ordering matter here: yes, indirectly - coalescing depends on being able to detect adjacent free chunks efficiently. ADDRSORT (default) keeps physically-neighboring chunks next to each other in the list, making coalescing cheap to perform. A size-sorted list would make coalescing far more expensive, since physically-adjacent chunks could be scattered anywhere in the list.

    Key takeaway: coalescing isn't optional polish - without it, any allocator strategy (best/worst/first) eventually degrades into unusable fragmentation given enough allocate/free cycles.

6. What happens when you change the percent allocated fraction -P
to higher than 50? What happens to allocations as it nears 100?
What about as the percent nears 0?
    `python3 malloc.py -n 100 -H 0 -p BEST -s 0 -P 95 -c` (95% allocs)
    `python3 malloc.py -n 100 -H 0 -p BEST -s 0 -P 5 -c` (5% allocs)

    P=95 (near 100%): Free List [ Size 0 ] - completely empty. With almost no frees, the fixed 100-byte heap gets fully consumed by allocations and never replenished. In a real allocator, this is exactly when it would need to grow the heap via sbrk - here it likely just means later allocation requests started failing once space ran out.

    P=5 (near 0%): 16 chunks remaining, sizes 1-52 bytes. Less extreme than expected - even a low 5% alloc rate over 100 ops still produces enough real allocations (~5) to fragment when freed, since almost the whole run is just repeatedly freeing (and fragmenting) whatever little gets allocated.

    Key takeaway: -P controls the allocation/free balance, and pushing to either extreme reveals a different failure mode - too many allocs starves the heap entirely (no free space left, matches Q5's fragmentation problem from a different angle), while too many frees mostly just leaves a fragmented but not-fully-consumed heap, since there's rarely much to free in the first place.

7. What kind of specific requests can you make to generate a highly fragmented free space? Use the -A flag to create fragmented free lists, and see how different policies and options change the organization of the free list.
    `python3 malloc.py -H 0 -A +10,+10,+10,+10,-1,-3 -p BEST -c`
    `python3 malloc.py -H 0 -A +10,+10,+10,+10,-1,-3 -p BEST -C -c`

    Pattern used: allocate 4x 10-byte chunks, then free indices 1 and 3 (checkerboard: used, free, used, free) - deliberately mimics the chapter's opening fragmentation example.

    Without coalescing: 3 chunks - [1010,10] [1030,10] [1040,60]
    With coalescing: 2 chunks - [1010,10] [1030,70] (last two merged; first stayed isolated)

    Key insight: coalescing only merges chunks that are PHYSICALLY ADJACENT in memory - it can't "reach across" a still-allocated chunk in between two free ones. [1010,10] stays stuck as an isolated island regardless of coalescing, because whatever's allocated right next to it (1020-1030) is still in use. Coalescing fixes fragmentation caused by consecutive frees, but does NOTHING for fragmentation caused by an interleaved/checkerboard allocation pattern like this one - the free space is genuinely scattered by the allocation pattern itself.

    This is the limit of coalescing: it's necessary but not sufficient. Some fragmentation is structural, baked into the actual pattern of what's allocated and freed, and no free-list bookkeeping trick can undo it - only actually freeing (or moving/compacting) the in-between allocated chunk would fix it, and compaction isn't possible for user-level heaps (per the chapter's own assumption) since pointers can't be safely relocated.