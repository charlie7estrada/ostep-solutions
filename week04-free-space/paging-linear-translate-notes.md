# Homework (Simulation)
In this homework, you will use a simple program, which is known as
paging-linear-translate.py, to see if you understand how simple
virtual-to-physical address translation works with linear page tables. See
the README for details.

# Questions
1. Before doing any translations, let’s use the simulator to study how
linear page tables change size given different parameters. Compute
the size of linear page tables as different parameters change. Some
suggested inputs are below; by using the -v flag, you can see how
many page-table entries are filled. First, to understand how linear
page table size changes as the address space grows, run with these
flags:
• -P 1k -a 1m -p 512m -v -n 0
• -P 1k -a 2m -p 512m -v -n 0
• -P 1k -a 4m -p 512m -v -n 0
    `python3 ... -P 1k -a 1m -p 512m -v -n 0 | grep -c '\['` → 1024 entries
    `python3 ... -P 1k -a 2m -p 512m -v -n 0 | grep -c '\['` → 2048 entries
    `python3 ... -P 1k -a 4m -p 512m -v -n 0 | grep -c '\['` → 4096 entries
    Result: Confirmed linear relationship - PTE count = address space size / page size. Doubling the address space exactly doubles the number of page table entries needed (1024→2048→4096). Bigger address space always means a proportionally bigger page table, regardless of how much of that space is actually used.

2. Then, let’s understand how linear page table size changes as page
size grows. Before running any of these, try to think about the expected trends. How should page-table size change as the address
space grows? As the page size grows? Why not use big pages in
general?
• -P 1k -a 1m -p 512m -v -n 0
• -P 2k -a 1m -p 512m -v -n 0
• -P 4k -a 1m -p 512m -v -n 0
    `-P 1k` → 1024 entries
    `-P 2k` → 512 entries
    `-P 4k` → 256 entries
    Result: Confirmed inverse relationship - doubling page size halves the number of entries needed (1024→512→256), following the same PTE count = asize/pagesize formula, just moving the denominator instead of the numerator this time.
    smaller pages = less internal fragmentation but bigger/slower page tables; 
    bigger pages = smaller/faster page tables but more wasted space per allocation.


3. Now let’s do some translations. Start with some small examples,
and change the number of pages that are allocated to the address
space with the -u flag. What happens as you increase the percentage of pages that are allocated in each address space?
• -P 1k -a 16k -p 32k -v -u 0
• -P 1k -a 16k -p 32k -v -u 25
• -P 1k -a 16k -p 32k -v -u 50
• -P 1k -a 16k -p 32k -v -u 75
• -P 1k -a 16k -p 32k -v -u 100
    -u 0: 0/16 valid entries, 0/5 translations succeeded - matches expectation exactly.
    -u 25: 4/16 valid entries (25% of 16), 1/5 translations succeeded - matches expectation.
    -u 50: 8/16 valid entries (50% of 16), 3/5 translations succeeded - matches expectation.
    -u 75 and -u 100: both showed 16/16 valid entries (100%), identical output - does NOT match the expected 12/16 for u=75. 

    Noted discrepancy: -u 75 should show 12 valid entries per the documented percentage behavior, consistent with the clean pattern seen at 0/25/50%. Instead it matched -u 100 exactly. Didn't have time to fully root-cause this - worth flagging as an open question rather than presenting a wrong explanation. The core concept (more -u = more valid pages = higher translation success rate) is clearly confirmed by the 0/25/50% data regardless.

4. Now let’s try some different random seeds, and some different (and
sometimes quite crazy) address-space parameters, for variety. Which
of these parameter combinations are unrealistic? Why?
• -P 8 -a 32 -p 1024 -v -s 1
• -P 8k -a 32k -p 1m -v -s 2
• -P 1m -a 256m -p 512m -v -s 3
    - P=8 bytes: page size WAY too small - real pages are KB-scale minimum. Would need enormous page tables to cover any real address space
    - P=1m: page size WAY too large - even tiny allocations would waste huge amounts of space (extreme internal fragmentation).
    - P=8k/a=32k/p=1m: most realistic of the three in shape, though still tiny/scaled-down for teaching purposes - 8KB pages aren't standard (4KB is) but aren't absurd either.

5. Use the program to try out some other problems. Can you find the
limits of where the program doesn’t work anymore? For example,
what happens if the address-space size is bigger than physical memory?
    `python3 paging-linear-translate.py -P 1k -a 1m -p 512k -v -n 5 -c`
    Result: "Error: physical memory size must be GREATER than address space size (for this simulation)" - the simulator explicitly refuses to run rather than producing garbage translations.

    Makes sense: this simulator's simplified model assumes an address space must fully fit within physical memory. Real systems obviously do support virtual address spaces larger than physical memory (that's the whole point of swapping/demand paging, covered in later chapters) - this simulator just doesn't model that complexity, so it fails fast with a clear error instead of pretending to handle it.