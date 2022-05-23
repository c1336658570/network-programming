/*
    有连接的UDP通信
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 30

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int clnt_sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t addr_sz; //多余变量！
    struct sockaddr_in serv_addr, from_addr; //不再需要from_addr！

    if (argc != 3)
    {
        printf("Usage:%s <IP> <PORT>\n", argv[0]);
        exit(-1);
    }

    clnt_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (clnt_sock == -1)
    {
        error_handling("socket() error");
    }

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], (void *)&serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(atoi(argv[2]));
    
    connect(clnt_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    while (1)
    {
        fputs("Insert message(q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
        {
            break;
        }

        /*
        sendto(clnt_sock, message, strlen(message), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        */
        write(clnt_sock, message, strlen(message));
        /*
        addr_sz = sizeof(from_addr);
        str_len = recvfrom(clnt_sock, message, BUF_SIZE, 0, (struct sockaddr *)&from_addr, &addr_sz);
        */
       str_len = read(clnt_sock, message, sizeof(message) - 1);
        message[str_len] = '\0';
        printf("Message from serv: %s", message);
    }
    close(clnt_sock);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputs("\n", stderr);
    exit(1);
}