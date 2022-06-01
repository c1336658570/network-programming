/*
禁用Nagle算法
int opt_val = 1;
setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&opt_val, sizeof(opt_val));
通过TCP_NODELAY的值查看Nagle算法的设置状态
int opt_val;
socklen_t opt_len;
opt_len = sizeof(opt_val);
getsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&opt_val, &opt_len);
如果正在使用Nagle算法，opt_val变量中会保存0，如果已禁用Nagle算法，则保存1
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>

int main(int argc, char *argv[])
{
    int sock;
    int opt_val = 1;
    //禁用Nagle算法
    setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&opt_val, sizeof(opt_val));

    //获取Nagle算法的设置状态
    int opt_val;
    socklen_t opt_len;
    opt_len = sizeof(opt_val);
    getsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&opt_val, &opt_len);

    return 0;
}