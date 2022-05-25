/*
readv使用

ssize_t readv(int fd, const struct iovec *iov, int iovcnt);
fd 传递接收数据的文件（或套间字）描述符
iov 包含数据保存位置和大小信息的iovec结构体数组的地址值
iovcnt 第二个参数中数组的长度
*/

#include <stdio.h>
#include <sys/uio.h>
#define BUF_SIZE 100

int main(int argc, char *argv[])
{
    struct iovec vec[2];
    char buf1[BUF_SIZE] = {
        0,
    };
    char buf2[BUF_SIZE] = {
        0,
    };
    int str_len;

    vec[0].iov_base = buf1;
    vec[0].iov_len = 5; //最多保存5个字节
    vec[1].iov_base = buf2;
    vec[1].iov_len = BUF_SIZE;

    str_len = readv(0, vec, 2);
    printf("Read bytes: %d \n", str_len);
    printf("First message: %s \n", buf1);
    printf("Second message: %s \n", buf2);
    return 0;
}

/*
Input:
    I like TCP/IP socket programming~
Output:
    Read bytes: 34 //字符串长度+'\n'
    First message: I lik
    Second message: e TCP/IP socket programming~
*/