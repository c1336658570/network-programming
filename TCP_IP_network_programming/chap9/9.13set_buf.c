/*
    SO_RCVBUF是输入缓冲大小相关可选事项，SO_SNDBUF是输出缓冲大小相关可选项。
    这两个可选项既可以读取当前I/O缓冲大小，也可以修改缓冲大小
    int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
    成功时返回0，失败时返回-1。
    int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
    成功时返回0，失败时返回-1。

    先设置I/O缓冲区大小
    再获取I/O缓冲区大小
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
void error_handling(char *message);

int main(int argc, char *argv[])
{
    int sock;
    int snd_buf = 1024 * 3, rcv_buf = 1024 * 3;
    int state;
    socklen_t len;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    state = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *)&rcv_buf, sizeof(rcv_buf));
    if (state)
        error_handling("setsockopt() error!");

    state = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *)&snd_buf, sizeof(snd_buf));
    if (state)
        error_handling("setsockopt() error!");

    len = sizeof(snd_buf);
    state = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *)&snd_buf, &len);
    if (state)
        error_handling("getsockopt() error!");

    len = sizeof(rcv_buf);
    state = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *)&rcv_buf, &len);
    if (state)
        error_handling("getsockopt() error!");

    printf("Input buffer size: %d \n", rcv_buf);
    printf("Output buffer size: %d \n", snd_buf);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}