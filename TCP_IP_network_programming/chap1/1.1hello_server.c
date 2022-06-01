#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void my_error(const char *str);

int main(int argc, char *argv[])
{
    int serv_sock;
    int clnt_sock;
    int ret = 0;

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    int clnt_addr_size;

    char message[] = "Hello World!";

    if (argc != 2)
    {
        printf("Usage:%s <port>\n", argv[0]);
        exit(-1);
    }

    bzero(&serv_addr, sizeof(serv_addr));
    bzero(&clnt_addr, sizeof(clnt_addr));

    ret = serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (ret == -1)
    {
        my_error("socket fails");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    ret = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret == -1)
    {
        my_error("bind fails");
    }

    ret = listen(serv_sock, 50);
    if (ret == -1)
    {
        my_error("listen fails");
    }

    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1)
    {
        my_error("accept fails");
    }

    write(clnt_sock, message, sizeof(message));
    close(clnt_sock);
    close(serv_sock);

    return 0;
}

void my_error(const char *str)
{
    perror(str);
    exit(-1);
}