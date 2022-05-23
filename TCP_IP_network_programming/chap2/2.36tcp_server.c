#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SERV_PORT 9190

void my_error(const char *str);

int main(void)
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    char msg[] = "Hello World!";

    bzero(&serv_addr, sizeof(serv_addr));
    bzero(&clnt_addr, sizeof(serv_addr));

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
    {
        my_error("socket fails");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERV_PORT);
    int ret = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret == -1)
    {
        my_error("bind fails");
    }

    ret = listen(serv_sock, 50);
    if (ret == -1)
    {
        my_error("listen fails");
    }

    int clnt_addr_len = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_len);
    if (clnt_sock == -1)
    {
        my_error("accept fails");
    }

    int len = sizeof(msg);
    for (int i = 0; i < len; ++i)
    {
        write(clnt_sock, &msg[i], 1);
    }

    printf("Function write call count : %d\n", len);
    close(serv_sock);
    close(clnt_sock);

    return 0;
}

void my_error(const char *str)
{
    perror(str);
    exit(-1);
}