# Homework (Simulation)
This fun little homework tests if you understand how a multi-level
page table works. And yes, there is some debate over the use of the term
“fun” in the previous sentence. The program is called, perhaps unsurprisingly: paging-multilevel-translate.py; see the README for
details.

# Questions
1. With a linear page table, you need a single register to locate the
page table, assuming that hardware does the lookup upon a TLB
miss. How many registers do you need to locate a two-level page
table? A three-level table?
    Linear: 1 register (page-table base register)
    Two-level: 1 register (page-directory base register) - the directory's own entries (stored in memory, not registers) point to each chunk of the page table
    Three-level: 1 register (top-level directory base) - same logic, extra levels are found via pointers in memory, not extra hardware registers

    Register count doesn't grow with the number of levels - only 1 register is ever needed, since the hardware just needs to know where to start the walk. What does grow with levels is the number of memory accesses required to complete a lookup (1 for linear, 2 for two-level, 3 for three-level)

2. Use the simulator to perform translations given random seeds 0,
1, and 2, and check your answers using the -c flag. How many
memory references are needed to perform each lookup?
    `python3 paging-multilevel-translate.py -s 0 -c`

    Traced Virtual Address 0x611c (successful translation):
    1. Access page directory (PDBR=108) → get PDE
    2. Access page table chunk (PFN from PDE) → get PTE  
    3. Access actual physical memory → get the real data

    = 3 memory references total for a successful 2-level lookup.

    Faults (e.g. 0x3da8, invalid PTE) only take 2 references - directory + PTE lookup - since the walk stops as soon as an invalid entry is found, before ever reaching step 3.

    Confirms the chapter's own math: 2-level lookup = 2 extra memory accesses beyond the actual data fetch (directory + PTE), matching "two additional memory accesses" stated directly in Ch20's text.

3. Given your understanding of how cache memory works, how do
you think memory references to the page table will behave in the
cache? Will they lead to lots of cache hits (and thus fast accesses?)
Or lots of misses (and thus slow accesses)?

    Mostly hits
    Reasoning: page directory/page table entries are a small structure relative to the data they describe - one PDE covers a whole chunk of PTEs, and one PTE covers an entire page of data. Many different data accesses within the same page (or same region of the address space) all reuse the SAME PTE/PDE. So even if actual data accesses look scattered across the address space, the underlying translation structures get hit repeatedly - strong locality, same principle as the TLB's own hit-rate behavior from Ch19. Page tables are small, frequently reused, and heavily concentrated - exactly the profile that caches well.