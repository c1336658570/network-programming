/*
    通过select函数实现I/O复用服务器端
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    struct timeval timeout;
    fd_set reads, cpy_reads;
    socklen_t clnt_addr_len;
    int fd_max, str_len, fd_num, i;
    char buf[BUF_SIZE];

    if (argc != 2)
    {
        printf("Usage : %s <port> \n", argv[0]);
        exit(-1);
    }

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
    {
        error_handling("socket() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    bzero(&clnt_addr, sizeof(clnt_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[1]));
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret == -1)
    {
        error_handling("bind() fails");
    }

    ret = listen(serv_sock, 5);
    if (ret == -1)
    {
        error_handling("listen() error");
    }

    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);
    fd_max = serv_sock;

    while (1)
    {
        cpy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;
        if ((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1)
        {
            break;
        }
        if (fd_num == 0)
        {
            continue;
        }

        for (i = 0; i < fd_max + 1; ++i)
        {
            if (FD_ISSET(i, &cpy_reads))
            {
                if (i == serv_sock)
                {
                    clnt_addr_len = sizeof(clnt_addr);
                    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_len);
                    FD_SET(clnt_sock, &reads);
                    if (fd_max < clnt_sock)
                    {
                        fd_max = clnt_sock;
                    }
                    printf("connected client: %d \n", clnt_sock);
                }
                else
                {
                    str_len = read(i, buf, BUF_SIZE);
                    if (str_len == 0) // 对端关闭
                    {
                        FD_CLR(i, &reads);
                        close(i);
                        printf("closed client: %d \n", i);
                    }
                    else
                    {
                        write(i, buf, str_len);
                    }
                }
            }
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
