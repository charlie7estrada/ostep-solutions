// 2. Write a program that opens a file (with the open() system call)
// and then calls fork() to create a new process. Can both the child
// and parent access the file descriptor returned by open()? What
// happens when they are writing to the file concurrently, i.e., at the
// same time?

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>


int main()
{
    int fd = open("output.txt", O_CREAT | O_WRONLY, 0644);
    int rc = fork();

    if (rc == 0) {
        // child
        for (int i = 0; i < 5; i++) {
            write(fd, "child\n", 6);
        }
    } else {
        // parent
        for (int i = 0; i < 5; i++) {
            write(fd, "parent\n", 7);
        }
    }

    return 0;
}

// Yes both child and parent can acess the file descriptor returned by open. Non-deterministic. If they are writing concurrently, the parent should go first.
