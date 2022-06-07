/*
如果在遇到换行符之前读取的字节数大于或等于（n–1），那么 readLine()函数会丢弃多
余的字节（包括换行符）。如果在前面的（n–1）字节中读取了换行符，那么在返回的字符串
中就会包含这个换行符。（因此可以通过检查在返回的 buffer 中结尾 null 字节前是否是一个换
行符来确定是否有字节被丢弃了。）采用这种方法之后，将输入以行为单位进行处理的应用程
序协议就不会将一个很长的行处理成多行了。当然，这可能会破坏协议，因为两端的应用程
序不再同步了。另一种做法是让 readLine()只读取足够的字节数来填充提供的缓冲器，而将到
下一行新行为止的剩余字节留给下一个 readLine()调用。在这种情况下，readLine()的调用者需
要处理读取部分行的情况。
*/

#ifndef READ_LINE_H
#define READ_LINE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#endif

ssize_t readLine(int fd, void *buffer, size_t n)
{
    ssize_t numRead;
    size_t totRead;
    char *buf;
    char ch;

    if (n <= 0 || buffer == NULL)
    {
        errno = EINVAL;
        return -1;
    }

    buf = buffer;

    totRead = 0;
    for (;;)
    {
        numRead = read(fd, &ch, 1);

        if (numRead == -1)
        {
            if (errno == EINTR)
            {
                continue;
            }
            else
            {
                return -1;
            }
        }
        else if (numRead == 0)
        {
            if (totRead == 0)
            {
                return 0;
            }
            else
            {
                break;
            }
        }
        else
        {
            if (totRead < n - 1)
            {
                totRead++;
                *buf++ = ch;
            }

            if (ch == '\n')
            {
                break;
            }
        }
    }

    *buf = '\0';
    return totRead;
}