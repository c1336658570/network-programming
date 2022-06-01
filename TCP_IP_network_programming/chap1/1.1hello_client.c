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
    char message[30];

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

    ret = read(sock, message, sizeof(message));
    if (ret == -1)
    {
        my_error("read fails");
    }

    printf("message from server : %s\n", message);
    close(sock);

    return 0;
}

void my_error(const char *str)
{
    perror(str);
    exit(-1);
}