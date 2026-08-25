# Homework (Simulation)
This simulation homework focuses on fork.py, a simple process creation simulator that shows how processes are related in a single “familial” tree. Read the relevant README for details about how to run the
simulator.

# Questions
1. Run ./fork.py -s 10 and see which actions are taken. Can you
predict what the process tree looks like at each step? Use the -c
flag to check your answers. Try some different random seeds (-s)
or add more actions (-a) to get the hang of it.
        `python3 fork.py -s 10`
        action 1: A/ b 
        action 2: A/ b, c
        action 3: A/ b
        action 4: A/ b, d
        action 5: A/ b, d, e

2. One control the simulator gives you is the fork_percentage, controlled by the -f flag. The higher it is, the more likely the next action is a fork; the lower it is, the more likely the action is an exit.
Run the simulator with a large number of actions (e.g., -a 100)
and vary the fork_percentage from 0.1 to 0.9. What do you
think the resulting final process trees will look like as the percentage changes? Check your answer with -c.
        `python3 fork.py -s 10 -a 100 -f 0.1 -c`
        `python3 fork.py -s 10 -a 100 -f 0.9 -c`
        The higher it is the bigger the final process tree is. 

3. Now, switch the output by using the -t flag (e.g., run ./fork.py
-t). Given a set of process trees, can you tell which actions were
taken?
        `python3 fork.py -s 10 -t`
        Yes, when a branch is no longer present in the tree it means it exited. If there is a new letter it means a new branch was created. This is obvious with just a tree.

4. One interesting thing to note is what happens when a child exits;
what happens to its children in the process tree? To study this, let’s
create a specific example: ./fork.py -A a+b,b+c,c+d,c+e,c-.
This example has process ’a’ create ’b’, which in turn creates ’c’,
which then creates ’d’ and ’e’. However, then, ’c’ exits. What do
you think the process tree should like after the exit? What if you
use the -R flag? Learn more about what happens to orphaned processes on your own to add more context
        The orphaned children would just move up one level, in this case be children of B instead of C.
        `python3 fork.py -A a+b,b+c,c+d,c+e,c- -c`
        Result: They actually become children of the root process 'a' by default, not b - meaning they're re-parented all the way to the top, not just one level up.
        `python3 fork.py -A a+b,b+c,c+d,c+e,c- -c -R`
        The -R flag is what makes them become children of the previous branch, or "reparenting"

5. One last flag to explore is the -F flag, which skips intermediate
steps and only asks to fill in the final process tree. Run ./fork.py
-F and see if you can write down the final tree by looking at the
series of actions generated. Use different random seeds to try this a
few times
        `python3 fork.py -F`
        Action: a forks b
        Action: a forks c 
        Action: b forks d
        Action: a forks e
        Action: c EXITS

        resulting tree:
        A
        |-B
        | |-D
        |-E

6. Finally, use both -t and -F together. This shows the final process
tree, but then asks you to fill in the actions that took place. By looking at the tree, can you determine the exact actions that took place?
In which cases can you tell? In which can’t you tell? Try some different random seeds to delve into this question
        `python3 fork.py -F -t`
        result tree:
        A
        |-B
          |-C
          | |-F
          |-D
          |-E
        which should mean:
        a forks b
        b forks c
        b forks d
        b forks e
        c forks f
        in that order. would get tricky if a branch forked new branches then exited with an -R flag. as those new processes would just like children of their parent and we miss out on the context of the reparenting