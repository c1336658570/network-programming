/*
    实现广播数据的sender
    ./14.21nwes_sender_brd 255.255.255.255 9190

广播（Broadcast）在“一次性向多个主机发送数据”，这一点可多播类似，但传输范围有区别。
多播即使在跨越不同网络的情况下，只要加入多播组就能接收到数据。相反，广播只能向同一网络中的主机传输数据。

广播是向同一网络中的所有主机传输数据的方法，广播也是基于UDP完成的。广播分为以下两种
直接广播（Directed Broadcast）
本地广播（Local Broadcast）

二者代码实现主要差别在于IP地址。直接广播的IP地址除了网络地址，其余主机地址全部设置为1。
换言之，向网络地址192.12.34中的所有主机传输数据时，可以向192.12.34.255传输。
换言之，可以采用直接广播方式向特定区域内所有主机传输数据。

反之，本地广播中使用的IP地址限定为255.255.255.255。例如，192.32.24网络中的主机向255.255.255传输数据时，
数据将传递到192.32.24网络中的所有主机。
默认生成的套间字会阻止广播，需要通过以下代码更改默认设置。
int send_sock;
int bcast = 1; //对变量进行初始化以将SO_BROADCAST选项信息改为1。
....
sebd_sock = socket(PF_INET, SOCK_DGRAM, 0);
....
setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, (void *)&bcast, sizeof(bcast));
只需要在sender中修改，receiver的实现不需要该过程。
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);

int main(int argc, char *argv[])
{
    int send_sock;
    struct sockaddr_in broad_adr;
    FILE *fp;
    char buf[BUF_SIZE];
    int so_brd = 1;

    if (argc != 3)
    {
        printf("Usage : %s <Boradcast IP> <PORT>\n", argv[0]);
        exit(1);
    }

    send_sock = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&broad_adr, 0, sizeof(broad_adr));
    broad_adr.sin_family = AF_INET;
    broad_adr.sin_addr.s_addr = inet_addr(argv[1]);
    broad_adr.sin_port = htons(atoi(argv[2]));

    setsockopt(send_sock, SOL_SOCKET,
               SO_BROADCAST, (void *)&so_brd, sizeof(so_brd));
    if ((fp = fopen("news.txt", "r")) == NULL)
        error_handling("fopen() error");

    while (!feof(fp))
    {
        fgets(buf, BUF_SIZE, fp);
        sendto(send_sock, buf, strlen(buf),
               0, (struct sockaddr *)&broad_adr, sizeof(broad_adr));
        sleep(2);
    }

    close(send_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}