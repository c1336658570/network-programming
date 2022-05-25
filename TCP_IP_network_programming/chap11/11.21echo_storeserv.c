/*
    改造第十章的echo_mpserv.c 使其可以将读取内容保存到文件中
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 100

void error_handling(char *message);
void read_childproc(int sig);

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    int fds[2];
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_addr_len;
    struct sigaction act;
    pid_t pid;
    int str_len, state;
    char buf[BUF_SIZE];

    if (argc != 2)
    {
        printf("Usage : %s <port> \n", argv[0]);
        exit(-1);
    }

    //注册信号处理函数，回收子进程
    memset(&act, 0, sizeof(act));
    act.sa_handler = read_childproc;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGCHLD, &act, NULL);

    //创建监听描述符
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
    {
        error_handling("socket() error");
    }

    //绑定地址结构
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret == -1)
    {
        error_handling("bind() error");
    }

    //设置监听
    ret = listen(serv_sock, 5);
    if (ret == -1)
    {
        error_handling("listen() error");
    }

    pipe(fds);
    pid = fork();

    if (pid == 0)
    {
        close(fds[1]); //关闭管道写端
        FILE *fp = fopen("echomsg.txt", "wt");
        char msgbuf[BUF_SIZE];
        int i, len;

        for (i = 0; i < 10; ++i)
        {
            len = read(fds[0], msgbuf, BUF_SIZE);
            fwrite((void *)msgbuf, 1, len, fp);
        }
        fclose(fp);
        return 0;
    }

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
        if (pid == 0)
        {
            close(serv_sock);
            while ((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0)
            {
                write(clnt_sock, buf, str_len);
                write(fds[1], buf, str_len);
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

//信号处理函数，遇到SIGCHLD信号，回收子进程
void read_childproc(int sig)
{
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, NULL, WNOHANG)) > 0)
    {
        printf("removed proc id: %d\n", pid);
    }
}