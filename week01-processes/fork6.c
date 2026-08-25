// 6. Write a slight modification of the previous program, this time using waitpid() instead of wait(). When would waitpid() be
// useful?

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int rc = fork();

    if (rc == 0) {
        // child
        printf("child, pid = %d\n", getpid());
    } else {
        // parent
        int wp = waitpid(rc, NULL, 0);
        printf("parent, pid = %d, waitpid returned = %d\n", getpid(), wp);
    }

    return 0;
}

// waitpid() lets you specify which child to wait for, instead of just "whichever child finishes first". 
// This would be useful if there were multiple children and we needed to wait for a specific child