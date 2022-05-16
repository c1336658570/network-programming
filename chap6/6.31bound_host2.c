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
    char message1[] = "Hi!";
    char message2[] = "I'm another UDP host!";
    char message3[] = "Nice to meet you";
    struct sockaddr_in serv_addr; 

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

    sendto(clnt_sock, message1, sizeof(message1), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)); 
    sendto(clnt_sock, message2, sizeof(message2), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)); 
    sendto(clnt_sock, message3, sizeof(message3), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)); 
    
    close(clnt_sock);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputs("\n", stderr);
    exit(1);
}