/*
输入
    ./8.2gethostbyname www.naver.com
输出
    Official name: e6030.a.akamaiedge.net
    Aliases 1: www.naver.com
    Aliases 2: www.naver.com.nheos.com
    Aliases 3: www.naver.com.edgekey.net
    Address type: AF_INET
    IP addr 1: 23.45.56.223

    ./8.2gethostbyname www.baidu.com
    Official name: www.a.shifen.com
    Aliases 1: www.baidu.com
    Address type: AF_INET
    IP addr 1: 14.215.177.39
    IP addr 2: 14.215.177.38
*/

//利用域名获取IP地址

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int i;
    struct hostent *host;
    if (argc != 2)
    {
        printf("Usage : %s <addr>\n", argv[0]);
        exit(-1);
    }

    host = gethostbyname(argv[1]);
    if (!host)
    {
        error_handling("gethost... error");
    }

    printf("Official name: %s \n", host->h_name);
    for (i = 0; host->h_aliases[i]; ++i)
    {
        printf("Aliases %d: %s \n", i + 1, host->h_aliases[i]);
    }
    printf("Address type: %s \n", (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");
    for (i = 0; host->h_addr_list[i]; ++i)
    {
        printf("IP addr %d: %s \n", i + 1, inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));
    }
    /*
    char *inet_ntoa(struct in_addr in);
    typedef uint32_t in_addr_t;
    struct in_addr {
        in_addr_t s_addr;
    };
    将整数形式的域名所对应的IP地址转为字符串类型，返回指针
    */

    return 0;
}

void error_handling(char *message)
{

    fputs(message, stderr);
    fputs("\n", stderr);
    exit(1);
}