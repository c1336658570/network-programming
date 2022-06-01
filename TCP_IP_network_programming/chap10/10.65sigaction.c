#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

void catch_sigint(int sig)
{
    printf("是否要退出程序，输入Y终止程序\n");
    char ch = getchar();
    if (ch == 'Y')
    {
        exit(0);
    }
}

int main(void)
{
    struct sigaction act;
    act.sa_handler = catch_sigint;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);

    while (1)
    {
        printf("111\n");
        sleep(1);
    }

    return 0;
}