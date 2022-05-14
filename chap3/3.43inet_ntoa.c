/*
    char *inet_ntoa(struct in_addr in);
                            成功返回转换的字符串地址值，失败时返回-1
功能与inet_aton相反
该函数返回的字符串在内部申请了空间并保存了字符串，该函数调用后，应立即将字符串信息复制到其他内存空间。防止再次调用该函数覆盖以前保存的信息char *inet_ntoa(struct in_addr in);
*/

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main(void)
{
    struct sockaddr_in addr1, addr2;
    char *str_ptr;
    char str_arr[20];

    addr1.sin_addr.s_addr = htonl(0x1020304);
    addr2.sin_addr.s_addr = htonl(0x1010101);

    str_ptr = inet_ntoa(addr1.sin_addr);
    strcpy(str_arr, str_ptr);
    printf("Dotted-Decimal notation1: %s \n", str_ptr);

    inet_ntoa(addr2.sin_addr);
    printf("Dotted-Decimal notation1: %s \n", str_ptr);
    printf("Dotted-Decimal notation1: %s \n", str_arr);

    return 0;
}