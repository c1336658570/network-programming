/*
    多进程服务器
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
void read_childproc(int sig);

int main(int argc, char *argv[])
{
    pid_t pid;
    struct sockaddr_in clnt_addr, serv_addr;
    int clnt_sock, serv_sock;
    socklen_t clnt_addr_len;
    struct sigaction act;
    int str_len, state;
    char buf[BUF_SIZE];

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    //设置信号处理函数
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD, &act, NULL);

    //创建套间字
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
    {
        error_handling("socket() error");
    }

    //设置端口复用
    int opt = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt));

    //绑定地址结构
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret == -1)
    {
        error_handling("bind() error");
    }

    ret = listen(serv_sock, 50);
    if (ret == -1)
    {
        error_handling("listen() error");
    }

    memset(&clnt_addr, 0, sizeof(clnt_addr));
    while (1)
    {
        clnt_addr_len = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_len);
        if (clnt_sock == -1)
        {
            continue;
        }
        else
        {
            puts("new client connected...");
        }

        pid = fork();
        if (pid == -1)
        {
            close(clnt_sock);
            continue;
        }
        else if (pid == 0) //子进程
        {
            close(serv_sock);
            while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0)
            {
                write(clnt_sock, buf, str_len);
            }

            close(clnt_sock);
            puts("client disconnected...");
            return 0;
        }
        else
        {
            close(clnt_sock);
        }
    }
    close(serv_sock);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputs("\n", stderr);
    exit(1);
}

void read_childproc(int sig)
{
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
    {
        printf("removed proc id: %d\n", pid);
    }
}