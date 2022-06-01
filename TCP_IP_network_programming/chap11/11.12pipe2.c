/*
    通过管道进行进程间双向通信
*/

#include <stdio.h>
#include <unistd.h>
#define BUF_SIZE 30

int main(int argc, char *argv[])
{
    int fds[2];
    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message";
    char buf[BUF_SIZE];
    pid_t pid;

    pipe(fds);
    pid = fork();

    if (pid == 0)
    {
        write(fds[1], str1, sizeof(str1));
        sleep(2); //防止子进程将自己写入到管道中的数据读取，导致父进程阻塞在read处
        read(fds[0], buf, BUF_SIZE);
        printf("Child proc output: %s \n", buf);
    }
    else
    {
        read(fds[0], buf, BUF_SIZE);
        printf("Parent proc output: %s \n", buf);
        write(fds[1], str2, sizeof(str2));
        sleep(3);
    }
    return 0;
}