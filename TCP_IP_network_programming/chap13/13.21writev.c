/*
writev使用

ssize_t writev(int fd, const struct iovec *iov, int iovcnt);
    成功时返回发送的字节数，失败时返回-1。
fd 表示数据传输对象的套间字文件描述符。但该函数并不只限于套间字，因此，可以像read函数一样向其传递文件或标准输出描述符。
iov iovec结构体数组的地址值，结构体iovec中包含待发送数据的位置和大小信息
iovcnt 向第二个参数传递的数组长度
struct iovec {
               void  *iov_base;    缓冲地址
               size_t iov_len;     缓冲大小
           };

*/

#include <stdio.h>
#include <sys/uio.h>

int main(int argc, char *argv[])
{
    struct iovec vec[2];
    char buf1[] = "ABCDEFG";
    char buf2[] = "1234567";
    int str_len;

    vec[0].iov_base = buf1;
    vec[0].iov_len = 3;
    vec[1].iov_base = buf2;
    vec[1].iov_len = 4;

    str_len = writev(1, vec, 2);
    puts("");
    printf("Write bytes: %d \n", str_len);
    return 0;
}