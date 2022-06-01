/*
通过管道实现进程间通信

#include <unistd.h>
// On Alpha, IA-64, MIPS, SuperH, and SPARC/SPARC64; see NOTES
struct fd_pair {
    long fd[2];
};
struct fd_pair pipe();

// On all other architectures
int pipe(int pipefd[2]);
*/

#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char *argv[])
{
    int fds[2];
    char str[] = "Who are you?";
    char buf[BUF_SIZE];
    pid_t pid;

    pipe(fds);
    pid = fork();
    if (pid == 0)
    {
        close(fds[0]); //子进程关闭读端
        write(fds[1], str, sizeof(str));
    }
    else
    {
        close(fds[1]); //父进程关闭写端
        read(fds[0], buf, BUF_SIZE);
        puts(buf);
    }
    return 0;
}