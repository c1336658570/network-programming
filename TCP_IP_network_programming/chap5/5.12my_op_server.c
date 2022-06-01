/*
    实现计算器服务器
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <math.h>

#define BUF_SIZE 1024

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;
    int i, j, k, sum;
    struct sockaddr_in serv_addr, clnt_addr;
    char buf[BUF_SIZE];
    int calculate[BUF_SIZE];

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    bzero(calculate, sizeof(calculate));
    bzero(&serv_sock, sizeof(serv_sock));
    memset(&clnt_sock, 0, sizeof(clnt_sock));

    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
    {
        error_handling("socket() error");
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    int ret = bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    if (ret == -1)
    {
        error_handling("bind() error");
    }

    ret = listen(serv_sock, 10);
    if (ret == -1)
    {
        error_handling("listen() error");
    }

    socklen_t clnt_addr_len;
    for (i = 0; i < 5; ++i)
    {
        j = 0;
        sum = 0;
        clnt_addr_len = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_len);
        if (clnt_sock == -1)
        {
            error_handling("accept() error");
        }

        // read函数返回值为读取的字符数，read会读取\n，而且不会添加'\0'
        while ((ret = read(clnt_sock, buf, BUF_SIZE - 1)) != -1 && ret != 0)
        {
            printf("read() buf = %s\n", buf);

            //如果是运算符，就进行运算
            if (!strcmp(buf, "+") || !strcmp(buf, "-") || !strcmp(buf, "*") || !strcmp(buf, "/"))
            {
                sum = calculate[0];
                if (!strcmp(buf, "+"))
                {
                    for (k = 1; k < j; ++k)
                    {
                        sum += calculate[k];
                    }
                }
                else if (!strcmp(buf, "-"))
                {
                    for (k = 1; k < j; ++k)
                    {
                        sum -= calculate[k];
                    }
                }
                else if (!strcmp(buf, "*"))
                {
                    for (k = 1; k < j; ++k)
                    {
                        sum *= calculate[k];
                    }
                }
                else
                {
                    for (k = 1; k < j; ++k)
                    {
                        sum /= calculate[k];
                    }
                }
                j = 0;
                write(clnt_sock, (char *)&sum, 4);
                printf("result = %d\n", sum);
            }
            else
            {
                int r;
                calculate[j] = 0;
                //将字符串转为整形，并存入数组中
                for (k = 0; k < ret - 1; ++k)
                {
                    if (buf[k] == '-')
                    {
                        continue;
                    }
                    r = buf[k] - '0';
                    for (int t = 0; t < ret - k - 2; t++)
                    {
                        r *= 10;
                    }
                    if (buf[0] == '-')
                    {
                        calculate[j] = r * -1;
                    }
                    else
                    {
                        calculate[j] += r;
                    }
                }
                j++;
            }
        }

        close(clnt_sock);
    }

    close(serv_sock);

    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
