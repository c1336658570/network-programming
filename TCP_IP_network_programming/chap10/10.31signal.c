/*
    信号处理函数

    发生信号将唤醒由于调用sleep函数而进入阻塞状态的进程。
*/

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig)
{
    if (sig == SIGALRM)
        puts("Time out!");

    alarm(2);
}
void keycontrol(int sig)
{
    if (sig == SIGINT)
        puts("CTRL+C pressed");
}

int main(int argc, char *argv[])
{
    int i;
    signal(SIGALRM, timeout);
    signal(SIGINT, keycontrol);
    alarm(2);

    for (i = 0; i < 3; i++)
    {
        puts("wait...");
        sleep(100); //有信号来，sleep结束
    }
    return 0;
}