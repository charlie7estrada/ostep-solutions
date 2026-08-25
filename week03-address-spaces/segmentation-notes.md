# Homework (Simulation)
This program allows you to see how address translations are performed
in a system with segmentation.

# Questions
1. First let’s use a tiny address space to translate some addresses. Here’s 
a simple set of parameters with a few different random seeds; can you translate the addresses?
• segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 0
(seeds 1, 2 skipped for time)
    `python3 segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 0`
    Virtual Address Trace
    VA  0: 0x0000006c (decimal:  108) --> PA or segmentation violation?
    VA  1: 0x00000061 (decimal:   97) --> PA or segmentation violation?
    VA  2: 0x00000035 (decimal:   53) --> PA or segmentation violation?
    VA  3: 0x00000021 (decimal:   33) --> PA or segmentation violation?
    VA  4: 0x00000041 (decimal:   65) --> PA or segmentation violation?

    Setup: asize=128, so top-bit cutoff is 64. VA < 64 = Segment 0, VA >= 64 = Segment 1.
    Seg 0 rule: if VA >= len0 (20) -> violation. else physical address = VA + base0 (0)
    Seg 1 rule: physical address = 512 + (VA - 128). if physical < base1 (492) -> violation

    VA0 (108, seg1): 512 + (108-128) = 492. 492 >= 492 -> VALID, physical=492
    VA1 (97, seg1): 512 + (97-128) = 481. 481 < 492 -> violation
    VA2 (53, seg0): 53 >= 20 -> violation
    VA3 (33, seg0): 33 >= 20 -> violation
    VA4 (65, seg1): 512 + (65-128) = 449. 449 < 492 -> violation

   `python3 segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 0 -c`
    Result: Confirmed - exact match on all 5

2. Now, let’s see if we understand this tiny address space we’ve constructed (using the parameters from the question above). What is the highest legal virtual address in segment 0? What about the lowest legal virtual address in segment 1? What are the lowest and highest illegal addresses in this entire address space? Finally, how would you run segmentation.py with the -A flag to test if you are right?
    Highest legal VA in Seg 0: 19
    Lowest legal VA in Seg 1: 108
    Illegal range spans 20-107
    Verify with -A flag: 
    `python3 segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -A 19,20,107,108 -c`
    Virtual Address Trace
    VA  0: 0x00000013 (decimal:   19) --> VALID in SEG0: 0x00000013 (decimal:   19)
    VA  1: 0x00000014 (decimal:   20) --> SEGMENTATION VIOLATION (SEG0)
    VA  2: 0x0000006b (decimal:  107) --> SEGMENTATION VIOLATION (SEG1)
    VA  3: 0x0000006c (decimal:  108) --> VALID in SEG1: 0x000001ec (decimal:  492)
    Result: confirmed limits.

3. Let’s say we have a tiny 16-byte address space in a 128-byte physical
memory. What base and bounds would you set up so as to get
the simulator to generate the following translation results for the
specified address stream: valid, valid, violation, ..., violation, valid,
valid? Assume the following parameters:
• segmentation.py -a 16 -p 128 -A 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 --b0 ? --l0 ? --b1 ? --l1 ?
    Address space (16 bytes) splits in half automatically: VA 0-7 = Segment 0, VA 8-15 = Segment 1.

    We want: VA 0,1 valid (first 2) / VA 2-13 violations / VA 14,15 valid (last 2)

    Segment 0 (grows positive, normal base/bounds logic): valid if VA < limit. 
    To make only VA 0,1 valid → len0 = 2

    Segment 1 (grows negative/backward): limit controls how many addresses are legal, 
    counting backward FROM THE TOP of its range (15), not from the bottom.
    To make only VA 14,15 (the last 2) valid → len1 = 2

    Both limits end up = 2 because we wantto match the "legal window" (2 addresses) 
    on each end of the address space - one counted from the bottom (Seg 0), one from the top (Seg 1).

   `python3 segmentation.py -a 16 -p 128 -A 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 --b0 0 --l0 2 --b1 100 --l1 2 -c`
    Virtual Address Trace
    VA  0: 0x00000000 (decimal:    0) --> VALID in SEG0: 0x00000000 (decimal:    0)
    VA  1: 0x00000001 (decimal:    1) --> VALID in SEG0: 0x00000001 (decimal:    1)
    VA  2: 0x00000002 (decimal:    2) --> SEGMENTATION VIOLATION (SEG0)
    VA  3: 0x00000003 (decimal:    3) --> SEGMENTATION VIOLATION (SEG0)
    VA  4: 0x00000004 (decimal:    4) --> SEGMENTATION VIOLATION (SEG0)
    VA  5: 0x00000005 (decimal:    5) --> SEGMENTATION VIOLATION (SEG0)
    VA  6: 0x00000006 (decimal:    6) --> SEGMENTATION VIOLATION (SEG0)
    VA  7: 0x00000007 (decimal:    7) --> SEGMENTATION VIOLATION (SEG0)
    VA  8: 0x00000008 (decimal:    8) --> SEGMENTATION VIOLATION (SEG1)
    VA  9: 0x00000009 (decimal:    9) --> SEGMENTATION VIOLATION (SEG1)
    VA 10: 0x0000000a (decimal:   10) --> SEGMENTATION VIOLATION (SEG1)
    VA 11: 0x0000000b (decimal:   11) --> SEGMENTATION VIOLATION (SEG1)
    VA 12: 0x0000000c (decimal:   12) --> SEGMENTATION VIOLATION (SEG1)
    VA 13: 0x0000000d (decimal:   13) --> SEGMENTATION VIOLATION (SEG1)
    VA 14: 0x0000000e (decimal:   14) --> VALID in SEG1: 0x00000062 (decimal:   98)
    VA 15: 0x0000000f (decimal:   15) --> VALID in SEG1: 0x00000063 (decimal:   99)
    Result: Confirmed - exact match on the target pattern.

4. Assume we want to generate a problem where roughly 90% of the
randomly-generated virtual addresses are valid (not segmentation
violations). How should you configure the simulator to do so?
Which parameters are important to getting this outcome?
    Key parameters: len0 and len1 (the limit/bounds registers) - these directly control 
    what fraction of each segment's half of the address space is legal.
    Each segment covers half the address space (asize/2). The fraction valid in that 
    segment = len / (asize/2).

    To get ~90% valid overall: set BOTH len0 and len1 to about 90% of asize/2.
    Example with asize=1000 (half=500): set len0 = 450 and len1 = 450 

   `python3 segmentation.py -a 1000 -p 4000 --b0 0 --l0 450 -B 3500 --l1 450 -n 1000 -c`

5. Can you run the simulator such that no virtual addresses are valid? How?
    Simplest approach: set len0 = 0 and len1 = 0.
    If the limit is 0, NO address can ever be < limit or reach a legal 
    physical address - every single address in both segments becomes a violation.

    `python3 segmentation.py -a 128 -p 512 --b0 0 --l0 0 -B 512 --l1 0 -n 10 -c`
