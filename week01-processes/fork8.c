// 8. Write a program that creates two children, and connects the standard output of one to the standard input of the other, using the
// pipe() system call.

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int fds[2];
    pipe(fds);

    int rc1 = fork();

    if (rc1 == 0) {
        // child 1: writer
        close(fds[0]);
        dup2(fds[1], STDOUT_FILENO);
        close(fds[1]);
        execlp("ls", "ls", NULL);
    } else {
        int rc2 = fork();

        if (rc2 == 0) {
            // child 2: reader
            close(fds[1]);
            dup2(fds[0], STDIN_FILENO);
            close(fds[0]);
            execlp("wc", "wc", NULL);
        } else {
            // parent
            close(fds[0]);
            close(fds[1]);
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}


// Built a pipe() connecting two child processes: child 1 runs ls with its stdout redirected into the pipe's write end (via dup2), child 2 runs wc with its stdin redirected from the pipe's read end. Parent closes both its own pipe fds immediately and waits for both children.
// Result: wc printed "11 11 105" - confirms ls's directory listing successfully flowed through the pipe into wc, exactly like running `ls | wc` in the shell manually.
// every process must close whichever pipe end it's not using. If the parent (or either child) keeps the write end open longer than needed, the reader can hang forever waiting for more data, since the pipe never gets the "no more writers" signal that tells the reader it's done.