// 4. Write a program that calls fork() and then calls some form of
// exec() to run the program /bin/ls. See if you can try all of the
// variants of exec(), including (on Linux) execl(), execle(),
// execlp(), execv(), execvp(), and execvpe(). Why do
// you think there are so many variants of the same basic call?

#include <stdio.h>
#include <unistd.h>

int main()
{
    int rc = fork();

    if (rc == 0) {
        // child
        // execl("/bin/ls", "ls", NULL);
        execlp("ls", "ls", NULL);
        printf("this should never print\n");  // proves exec replaced this process
    } else {
        // parent
        printf("parent still running, pid = %d\n", getpid());
    }

    return 0;
}

// execl - first (path + listed args)
// execlp worked exactly the same as execl, different lookup mechanism (searchable name + listed args)
// execv works the same (path + array of args).
// all of these variations exist to allow the caller to be as specific as they need to be