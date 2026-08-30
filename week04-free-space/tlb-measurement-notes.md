# Homework (Measurement)
**Note: Q2-4 (writing tlb.c, the measurement script, and generating a real graph) are skipped - treating this as conceptual/methodology review rather than a full build. Q1, 5, 6, 7 are answered below since they're not code-dependent.**


In this homework, you are to measure the size and cost of accessing
a TLB. The idea is based on work by Saavedra-Barrera [SB92], who developed a simple but beautiful method to measure numerous aspects of
cache hierarchies, all with a very simple user-level program. Read his
work for more details.
The basic idea is to access some number of pages within a large data
structure (e.g., an array) and to time those accesses. For example, let’s say
the TLB size of a machine happens to be 4 (which would be very small,
but useful for the purposes of this discussion). If you write a program
that touches 4 or fewer pages, each access should be a TLB hit, and thus
relatively fast. However, once you touch 5 pages or more, repeatedly in a
loop, each access will suddenly jump in cost, to that of a TLB miss.
The basic code to loop through an array once should look like this:
int jump = PAGESIZE / sizeof(int);
for (i = 0; i < NUMPAGES * jump; i += jump)
a[i] += 1;
In this loop, one integer per page of the array a is updated, up to the
number of pages specified by NUMPAGES. By timing such a loop repeatedly (say, a few hundred million times in another loop around this one, or
however many loops are needed to run for a few seconds), you can time
how long each access takes (on average). By looking for jumps in cost as
NUMPAGES increases, you can roughly determine how big the first-level
TLB is, determine whether a second-level TLB exists (and how big it is if
it does), and in general get a good sense of how TLB hits and misses can
affect performance.
1 4 16 64 256 1024
0
20
40
60
80
TLB Size Measurement
Number Of Pages
Time Per Access (ns)
Figure 19.5: Discovering TLB Sizes and Miss Costs
© 2008–23, ARPACI-DUSSEAU
THREE
EASY
PIECES
16 PAGING: FASTER TRANSLATIONS (TLBS)
Figure 19.5 (page 15) shows the average time per access as the number
of pages accessed in the loop is increased. As you can see in the graph,
when just a few pages are accessed (8 or fewer), the average access time
is roughly 5 nanoseconds. When 16 or more pages are accessed, there is
a sudden jump to about 20 nanoseconds per access. A final jump in cost
occurs at around 1024 pages, at which point each access takes around 70
nanoseconds. From this data, we can conclude that there is a two-level
TLB hierarchy; the first is quite small (probably holding between 8 and
16 entries); the second is larger but slower (holding roughly 512 entries).
The overall difference between hits in the first-level TLB and misses is
quite large, roughly a factor of fourteen. TLB performance matters!


# Questions
1. For timing, you’ll need to use a timer (e.g., gettimeofday()).
How precise is such a timer? How long does an operation have
to take in order for you to time it precisely? (this will help determine how many times, in a loop, you’ll have to repeat a page access
in order to time it successfully)
    gettimeofday() typically has microsecond-level precision, but a single memory access (TLB hit ~5ns per the book's example) is far too fast to measure accurately with one call - you'd be measuring timer overhead, not the actual access. This is why the homework's suggested approach loops the access "a few hundred million times" - by timing a huge batch of repeated accesses and dividing by the count, the per-access cost becomes measurable even though a single access is way below the timer's resolution.


2. Write the program, called tlb.c, that can roughly measure the cost
of accessing each page. Inputs to the program should be: the number of pages to touch and the number of trials.
    #skip

3. Now write a script in your favorite scripting language (bash?) to
run this program, while varying the number of pages accessed from
1 up to a few thousand, perhaps incrementing by a factor of two
per iteration. Run the script on different machines and gather some
data. How many trials are needed to get reliable measurements?
    #skip

4. Next, graph the results, making a graph that looks similar to the
one above. Use a good tool like ploticus or even zplot. Visualization usually makes the data much easier to digest; why do you
think that is?
    #skip

5. One thing to watch out for is compiler optimization. Compilers
do all sorts of clever things, including removing loops which increment values that no other part of the program subsequently uses.
How can you ensure the compiler does not remove the main loop
above from your TLB size estimator?
    The compiler may notice the loop's result (a[i] += 1) is never used afterward and just delete the whole loop as dead code. Common fixes: mark the array as `volatile` (tells the compiler "don't assume this doesn't matter, something outside could observe it"), or actually use/print the final result after the loop so the compiler can't prove it's discardable, or use compiler-specific flags/pragmas that disable that specific optimization.


6. Another thing to watch out for is the fact that most systems today
ship with multiple CPUs, and each CPU, of course, has its own TLB
hierarchy. To really get good measurements, you have to run your
code on just one CPU, instead of letting the scheduler bounce it
from one CPU to the next. How can you do that? (hint: look up
“pinning a thread” on Google for some clues) What will happen if
you don’t do this, and the code moves from one CPU to the other?
    Modern machines have multiple CPUs, each with its OWN separate TLB. If the OS scheduler moves your program between CPUs mid-measurement, you'd effectively be measuring a mix of "warm" and "cold" TLBs from different cores - garbage, inconsistent data. Fix: "pin" the thread/process to one specific CPU core for the whole run (on Linux, this is done via taskset or the sched_setaffinity() system call), so the TLB you're measuring stays consistent throughout.


7. Another issue that might arise relates to initialization. If you don’t
initialize the array a above before accessing it, the first time you
access it will be very expensive, due to initial access costs such as
demand zeroing. Will this affect your code and its timing? What
can you do to counterbalance these potential costs?
    If you access a freshly-allocated array without initializing it first, the very first touch to each page can be artificially expensive - the OS has to actually zero that physical page before handing it over (a real mechanism, "demand zeroing," not something covered in depth yet but real). This first-touch cost would contaminate your TLB measurements, making early page accesses look slower than they really are for TLB reasons. Fix: do a full "warm-up" pass over the entire array BEFORE starting your actual timed measurements, so all the demand-zeroing cost happens up front and is excluded from the real TLB timing data.
