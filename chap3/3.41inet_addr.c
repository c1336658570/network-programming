/*
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int inet_aton(const char *cp, struct in_addr *inp);
                            成功返回1（true），失败返回0（false）
功能与inet_addr完全相同
string 含有需转换的IP地址信息的字符串地址值     	addr 将转换结果保存在in_addr结构体变量的地址值

in_addr_t inet_addr(const char *cp);
                            传入点分十进制的IP，成功返回32位大端整数型值，失败返回INADDR_NONE

in_addr_t inet_network(const char *cp);

char *inet_ntoa(struct in_addr in);
                            成功返回转换的字符串地址值，失败时返回-1

struct in_addr inet_makeaddr(in_addr_t net, in_addr_t host);

in_addr_t inet_lnaof(struct in_addr in);

in_addr_t inet_netof(struct in_addr in);#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int inet_aton(const char *cp, struct in_addr *inp);

in_addr_t inet_addr(const char *cp);
                            传入点分十进制的IP，成功返回32位大端整数型值，失败返回INADDR_NONE

in_addr_t inet_network(const char *cp);

char *inet_ntoa(struct in_addr in);

struct in_addr inet_makeaddr(in_addr_t net, in_addr_t host);

in_addr_t inet_lnaof(struct in_addr in);

in_addr_t inet_netof(struct in_addr in);
*/

#include <stdio.h>
#include <arpa/inet.h>

int main(void)
{
    char *addr1 = "1.2.3.4";
    char *addr2 = "1.2.3.256";

    uint32_t conv_addr = inet_addr(addr1);
    if (conv_addr == INADDR_NONE)
    {
        printf("Error occured!\n");
    }
    else
    {
        printf("Network ordered integer addr: %#x \n", conv_addr);
    }

    conv_addr = inet_addr(addr2);
    if (conv_addr == INADDR_NONE)
    {
        printf("Error occured!\n");
    }
    else
    {
        printf("Network ordered integer addr: %#x \n", conv_addr);
    }

    return 0;
}