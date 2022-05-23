/*
    实现计算器客户端
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 1024

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int sock;
    char message[BUF_SIZE];
    int str_len;
    struct sockaddr_in serv_addr;

    if (argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        error_handling("socket() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], (void *)&serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(atoi(argv[2]));

    int ret = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret == -1)
    {
        error_handling("connect() fails");
    }
    else
    {
        puts("Connected...");
    }

    while (1)
    {
        fputs("Input message(Q to quit): \n", stdout);
        // read函数返回值为读取的字符数，read会读取\n，而且不会添加'\0'
        ret = read(STDIN_FILENO, message, BUF_SIZE - 1);
        message[ret - 1] = 0;

        if (!strcmp(message, "Q") || !strcmp(message, "q"))
        {
            break;
        }

        write(sock, message, ret);
        if (!strcmp(message, "+") || !strcmp(message, "-") || !strcmp(message, "*") || !strcmp(message, "/"))
        {
            str_len = read(sock, message, BUF_SIZE - 1);
            printf("Message from server: %d\n", *((int *)message));
        }
    }
    close(sock);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}