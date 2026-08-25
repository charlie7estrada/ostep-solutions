// 5. Now write a program that uses wait() to wait for the child process
// to finish in the parent. What does wait() return? What happens if
// you use wait() in the child?

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int rc = fork();

    if (rc == 0) {
        // child
        int wc = wait(NULL);
        printf("child, pid = %d, wait returned = %d\n", getpid(), wc);
    } else {
        // parent
        int wc = wait(NULL); 
        printf("parent, pid = %d, wait returned = %d\n", getpid(), wc);
    }

    return 0;
}

// wait() pauses the calling process until one of its children finishes.
// wait() inside the child branch probably wont have an effect if they dont have children of their own ? 
    // wait() returns -1 when called by a process with no children to wait for. -1 is C's standard convention for "this call failed". The child correctly reports that there was nothing for it to wait on.