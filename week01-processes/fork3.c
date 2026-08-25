// 3. Write another program using fork(). The child process should
// print “hello”; the parent process should print “goodbye”. You should
// try to ensure that the child process always prints first; can you do
// this without calling wait() in the parent?

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


int main()
{
    int rc = fork();

    if (rc == 0) {
        // child
        printf("hello\n");
    } else {
        // parent
        printf("goodbye\n");
    }

    return 0;
}

// Looks like parent always goes first if I dont use wait.