/*
客户端受到用户输入的文件名，发送给服务器，服务器打开文件，并将文件内容发送给客户端
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void my_error(const char *str);

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char pathname[256];
    char message[BUFSIZ];

    if (argc != 3)
    {
        printf("Usage:%s <IP> <PORT>\n", argv[0]);
        exit(-1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1)
    {
        my_error("socket fails");
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    int ret = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret == -1)
    {
        my_error("connect fails");
    }

    printf("请输入文件名：\n");
    ret = read(STDIN_FILENO, pathname, sizeof(pathname) - 1);
    if (ret == -1)
    {
        my_error("read() fails");
    }
    pathname[ret - 1] = '\0';

    ret = write(sock, pathname, strlen(pathname) + 1);
    if (ret == -1)
    {
        my_error("write() fails");
    }

    while ((ret = read(sock, message, BUFSIZ - 1)) != -1 && ret != 0)
    {
        write(STDOUT_FILENO, message, ret);
    }

    close(sock);

    return 0;
}

void my_error(const char *str)
{
    perror(str);
    exit(-1);
}