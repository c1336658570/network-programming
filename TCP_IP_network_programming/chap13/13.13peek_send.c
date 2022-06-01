/*
同时设置SMG_PEEK选项和MSG_DONTWAIT选项，以验证输入缓冲中是否存在接收的数据。
设置MSG_PEEK选项并调用recv函数时，即使读取了输入缓冲的数据也不会删除。
因此，该选项通常与MSG_DONTWAIT合作，用于调用以非阻塞方式验证待读数据存在与否的函数。
*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
void error_handling(char *message);

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in send_adr;
    if (argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&send_adr, 0, sizeof(send_adr));
    send_adr.sin_family = AF_INET;
    send_adr.sin_addr.s_addr = inet_addr(argv[1]);
    send_adr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr *)&send_adr, sizeof(send_adr)) == -1)
        error_handling("connect() error!");

    write(sock, "123", strlen("123"));
    close(sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}