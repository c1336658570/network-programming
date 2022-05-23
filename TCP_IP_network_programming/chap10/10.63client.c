/*
    回声客户端的I/O程序分割
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);

int main(int argc, char *argv[])
{
    int sock;
    pid_t pid;
    char buf[BUF_SIZE];
    struct sockaddr_in serv_addr;

    if (argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(-1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    int ret = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret == -1)
    {
        error_handling("connect() error");
    }

    pid = fork();
    if (pid == 0)
    {
        write_routine(sock, buf);
    }
    else
    {
        read_routine(sock, buf);
    }

    close(sock);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputs("\n", stderr);
    exit(1);
}

void read_routine(int sock, char *buf)
{
    read(sock, buf, BUF_SIZE);
    if (!strncmp(buf, (char *)&sock, 4))
    {
        printf("父进程的文件描述符为%d\n", *(int *)buf);
        printf("子进程的文件描述符为%d\n", sock);
        printf("相等\n");
    }
    else
    {
        printf("父进程的文件描述符为%d\n", *(int *)buf);
        printf("子进程的文件描述符为%d\n", sock);
        printf("不相等\n");
    }
}
void write_routine(int sock, char *buf)
{
    strncpy(buf, (char *)&sock, 4);
    write(sock, buf, 4);
}