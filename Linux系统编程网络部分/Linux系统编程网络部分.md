# Linux系统编程

## chap56 SOCKET：介绍

```c
socket 是一种 IPC 方法，它允许位于同一主机（计算机）或使用网络连接起来的不同主机上的应用程序之间交换数据。

int socket(int domain, int type, int protocol);
通信 domain
socket 存在于一个通信 domain 中，它确定：
识别出一个 socket 的方法（即 socket“地址”的格式）；

通信范围（即是在位于同一主机上的应用程序之间还是在位于使用一个网络连接起来的不同主机上的应用程序之间）。

现代操作系统至少支持下列 domain。
UNIX (AF_UNIX) domain 允许在同一主机上的应用程序之间进行通信。（POSIX.1g 使用名称 AF_LOCAL 作为 AF_UNIX 的同义词，但 SUSv3 并没有使用这个名称。）

IPv4 (AF_INET) domain 允许在使用因特网协议第 4 版（IPv4）网络连接起来的主机上
的应用程序之间进行通信。

IPv6 (AF_INET6) domain 允许在使用因特网协议第 6 版（IPv6）网络连接起来的主机
上的应用程序之间进行通信。尽管 IPv6 被设计成了 IPv4 接任者，但目前后一种协议
仍然是使用最广的协议。
```

| Domain   | 执行的通信 | 应用程序间的通信                  | 地址格式                          | 地址结构     |
| -------- | ---------- | --------------------------------- | --------------------------------- | ------------ |
| AF_UNIX  | 内核中     | 同一主机                          | 路径名                            | sockaddr_un  |
| AF_INET  | 通过 IPv4  | 通过 IPv4 网络连接<br/>起来的主机 | 32 位 IPv4 地址<br/>+16 位端口号  | sockaddr_in  |
| AF_INET6 | 通过 IPv6  | 通过 IPv6 网络连接<br/>起来的主机 | 128 位 IPv6 地址<br/>+16 位端口号 | sockaddr_in6 |

```c
流 socket（SOCK_STREAM）提供了一个可靠的双向的字节流通信信道。在这段描述中
的术语的含义如下。
可靠的：表示可以保证发送者传输的数据会完整无缺地到达接收应用程序（假设网络
链接和接收者都不会崩溃）或收到一个传输失败的通知。
双向的：表示数据可以在两个 socket 之间的任意方向上传输。
字节流：表示与管道一样不存在消息边界的概念（参见 44.1 节）。
```

### socket系统调用

```c
关键的 socket 系统调用包括以下几种。
socket()系统调用创建一个新 socket。
bind()系统调用将一个 socket 绑定到一个地址上。通常，服务器需要使用这个调用来
将其 socket 绑定到一个众所周知的地址上使得客户端能够定位到该 socket 上。
listen()系统调用允许一个流 socket 接受来自其他 socket 的接入连接。
accept()系统调用在一个监听流 socket 上接受来自一个对等应用程序的连接，并可选地
返回对等 socket 的地址。
connect()系统调用建立与另一个 socket 之间的连接。

socket I/O 可以使用传统的 read()和 write()系统调用或使用一组 socket 特有的系统调用
（如send()、recv()、sendto()以及 recvfrom()）来完成。在默认情况下，这些系统调用在 I/O 操作无
法被立即完成时会阻塞。通过使用 fcntl() F_SETFL 操作（5.3 节）来启用 O_NONBLOCK 打
开文件状态标记可以执行非阻塞 I/O。
在 Linux 上可以通过调用 ioctl(fd, FIONREAD, &cnt)来获取文件描述符 fd 引用的流
socket 中可用的未读字节数。对于数据报 socket 来讲，这个操作会返回下一个未读数据报
中的字节数（如果下一个数据报的长度为零的话就返回零）或在没有未决数据报的情况下
返回 0。这种特性没有在 SUSv3 中予以规定。
```

### 创建socket：socket()

```c
int socket(int domain, int type, int protocol);
domain 参数指定了 socket 的通信 domain。type 参数指定了 socket 类型。这个参数通常在
创 建 流 socket 时 会 被 指 定 为 SOCK_STREAM，而 在 创 建 数据 报 socket 时 会 被 指 定为
SOCK_DGRAM。

Linux 为 type 参数提供了第二种用途，即允许两个非标准的标记与
socket 类型取 OR。SOCK_CLOEXEC 标记会导致内核为新文件描述符启用 close-on-exec 标
记（FD_CLOEXEC）。这个标记之所以有用的原因与 4.3.1 节中描述的 open() O_CLOEXEC
标记有用的原因是一样的。SOCK_NONBLOCK 标记导致内核在底层打开着的文件描述符
上设置 O_NONBLOCK 标记，这样后面在该 socket 上发生的 I/O 操作就变成非阻塞了，
```

### 将socket绑定到地址：bind

```c
int bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen);

sockfd 参数是在上一个 socket()调用中获得的文件描述符。 addr 参数是一个指针，它指向了一个
指定该 socket 绑定到的地址的结构。传入这个参数的结构的类型取决于 socket domain。addrlen 参数
指定了地址结构的大小。addrlen 参数使用的 socklen_t 数据类型在 SUSv3 被规定为一个整数类型。
    
除了将一个服务器的 socket 绑定到一个众所周知的地址之外还存在其他做法。例如，对于一个 Internet domain socket 来讲，服务器可以不调用 bind()而直接调用 listen()，这将会导致内核为该 socket 选择一个临时端口。（在 58.6.1 节中将会介绍临时端口。）之后服务器可以使用 getsockname()（61.5 节）来获取 socket 的地址。在这种场景中，服务器必须要发布其地址使得客户端能够知道如何定位到服务器的 socket。这种发布可以通过向一个中心目服务应用程序注册服务器的地址来完成，之后客户端可以通过这个服务来获取服务器的地址。
```

### 监听接入连接：listen()

```c
int listen(int s, int backlog);
无法在一个已连接的 socket（即已经成功执行 connect()的 socket 或由 accept()调用返回的
socket）上执行 listen()。
要理解 backlog 参数的用途首先需要注意到客户端可能会在服务器调用 accept()之前调用connect()。这种情况是有可能会发生的，如服务器可能正忙于处理其他客户端。这将会产生一个未决的连接,内核必须要记录所有未决的连接请求的相关信息，这样后续的 accept()就能够处理这些请求了。backlog 参数允许限制这种未决连接的数量。在这个限制之内的连接请求会立即成功。（对于 TCP socket 来讲事情就稍微有点复杂了，具体会在 61.6.4 节中进行介绍。）之外的连接请求就会阻塞直到一个未决的连接被接受（通过 accept()），并从未决连接队列删除为止。
```

### 接受连接：accept()

```c
int accept(int s, struct sockaddr *addr, socklen_t *addrlen);
ccept()系统调用在文件描述符 sockfd 引用的监听流 socket 上接受一个接入连接。如果在调用 accept()时不存在未决的连接，那么调用就会阻塞直到有连接请求到达为止。
理解 accept()的关键点是它会创建一个新 socket，并且正是这个新 socket 会与执行 connect()的对等 socket 进行连接。accept()调用返回的函数结果是已连接的 socket 的文件描述符。

从内核 2.6.28 开始，Linux 支持一个新的非标准系统调用 accept4()。这个系统调用执行的任务与 accept()相同，但支持一个额外的参数 flags，而这个参数可以用来改变系统调用的行为。目前系统支持两个标记：SOCK_CLOEXEC 和 SOCK_NONBLOCK。SOCK_CLOEXEC 标记导致内核在调用返回的新文件描述符上启用 close-on-exec 标记（FD_CLOEXEC）。这个标记之所以有SOCK_NONBLOCK用的原因与4.3.1节中描述的open() O_CLOEXEC标记有用的原因是一样的。标记导致内核在底层打开着的文件描述上启用 O_NONBLOCK 标记，这样在该 socket 上发生的后续 I/O 操作将会变成非阻塞了。
```

### 连接到对等 socket：connect()

```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
connect()系统调用将文件描述符 sockfd 引用的主动 socket 连接到地址通过 addr 和 addrlen
指定的监听 socket 上。
```

### 流 socket I/O

```c
一对连接的流 socket 在两个端点之间提供了一个双向通信信道。

连接流 socket 上 I/O 的语义与管道上 I/O 的语义类似。要执行 I/O 需要使用 read()和 write()系统调用（或在 61.3 节中描述的 socket 特有的 send()和 recv()调用）。由于 socket 是双向的，因此在连接的两端都可以使用这两个调用。
一个 socket 可以使用 close()系统调用来关闭或在应用程序终止之后关闭。之后当对等应用程序试图从连接的另一端读取数据时将会收到文件结束（当所有缓冲数据都被读取之后）。如果对等应用程序试图向其 socket 写入数据，那么它就会收到一个 SIGPIPE信号，并且系统调用会返回 EPIPE 错误。在 44.2 节中曾提及过处理这种情况的常见方式是忽略 SIGPIPE 信号并通过 EPIPE 错误找出被关闭的连接。
```

### 连接终止：close()

```c
终止一个流 socket 连接的常见方式是调用 close()。如果多个文件描述符引用了同一个socket，那么当所有描述符被关闭之后连接就会终止。
```


### 数据报socket

```c
数据报 socket 的运作类似于邮政系统。
1. socket()系统调用等价于创建一个邮箱。（这里假设一个系统与一些国家的农村中的邮政服务类似，取信和送信都是在邮箱中发生的。）所有需要发送和接收数据报的应用程序都需要使用 socket()创建一个数据报 socket。
2.为允许另一个应用程序发送其数据报（信），一个应用程序需要使用 bind()将其 socket 绑定到一个众所周知的地址上。一般来讲，一个服务器会将其 socket 绑定到一个众所周知的地址上，而一个客户端会通过向该地址发送一个数据报来发起通信。（在一些 domain中——特别是 UNIX domain——客户端如果想要接受服务器发送来的数据报的话可能还需要使用 bind()将一个地址赋给其 socket。）
3. 要发送一个数据报，一个应用程序需要调用 sendto()，它接收的其中一个参数是数据报发送到的 socket 的地址。这类似于将收信人的地址写到信件上并投递这封信。
4. 为接收一个数据报，一个应用程序需要调用 recvfrom()，它在没有数据报到达时会阻塞。由于 recvfrom()允许获取发送者的地址，因此可以在需要的时候发送一个响应。（这在发送者的 socket 没有绑定到一个众所周知的地址上时是有用的，客户端通常是会碰到这种情况。）这里对这个比喻做了一点延伸，因为已投递的信件上是无需标记上发送者的地址的。
5.当不再需要 socket 时，应用程序需要使用 close()关闭 socket。与邮政系统一样，当从一个地址向另一个地址发送多个数据报（信）时是无法保证它们按照被发送的顺序到达的，甚至还无法保证它们都能够到达。数据报还新增了邮政系统所不具备的一个特点：由于底层的联网协议有时候会重新传输一个数据包，因此同样的数据包可能会多次到达。
```

### 交换数据报：recvfrom 和 sendto()

```c
#include <sys/types.h>
#include <sys/socket.h>

ssize_t recv(int sockfd, void *buf, size_t len, int flags);

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);

#include <sys/types.h>
#include <sys/socket.h>

int send(int s, const void *msg, size_t len, int flags);
int  sendto(int s, const void *msg, size_t len, int flags, const struct sockaddr *to, socklen_t tolen);
```

### 在数据报 socket 上使用 connect()

```c
尽管数据报 socket 是无连接的，但在数据报 socket 上应用 connect()系统调用仍然是起作用的。在数据报 socket 上调用 connect()会导致内核记录这个 socket 的对等 socket 的地址。术语已连接的数据报 socket 就是指此种 socket。术语非连接的数据报 socket是指那些没有调用connect()的数据报 socket（即新数据报 socket 的默认行为）。
    
当一个数据报 socket 已连接之后：
数据报的发送可在 socket 上使用 write()（或 send()）来完成并且会自动被发送到同样
的对等 socket 上。与 sendto()一样，每个 write()调用会发送一个独立的数据报；

在这个 socket 上只能读取由对等 socket 发送的数据报。

注意 connect()的作用对数据报 socket 是不对称的。上面的论断只适用于调用了 connect(),并不适用于它连接的远程 socket（除非对等应用程序在其 socket 上也调用了connect()）。

通过再发起一个 connect()调用可以修改一个已连接的数据报 socket 的对等 socket。此外，通过指定一个地址族（如 UNIX domain 中的 sun_family 字段）为 AF_UNSPEC 的地址结构还可以解除对等关联关系。但需要注意的是，其他很多 UNIX 实现并不支持将 AF_UNSPEC 用于这种用途。
    
为一个数据报 socket 设置一个对等 socket，这种做法的一个明显优势是在该 socket 上传输数据时可以使用更简单的 I/O 系统调用，即无需使用指定了 dest_addr 和 addrlen 参数的sendto()，而只需要使用 write()即可。

```

### 总结

```c
socket 允许在同一主机或通过一个网络连接起来的不同主机上的应用程序之间通信。

一个 socket 存在于一个通信 domain 中，通信 domain 确定了通信范围和用来标识 socket的地址格式。SUSv3 规定了 UNIX（AF_UNIX）、IPv4（AF_INET）以及 IPv6（AF_INET6）通信 domain。

大多数应用程序使用流 socket 和数据报 socket 中的一种。流 socket（SOCK_STREAM）为两个端之间提供了一颗可靠的、双向的字节流通信信道。数据报 socket（SOCK_DGRAM）提供了不可靠的、无连接的、面向消息的通信。

一个典型的流 socket 服务器会使用 socket()创建其 socket，然后使用 bind()将这个 socket绑定到一个众所周知的地址上。服务器接着调用 listen()以允许在该 socket 上接受连接。监听socket 上的客户端连接是通过 accept()来接受的，它将返回一个与客户端的 socket 进行连接的新 socket 的文件描述符。一个典型的流 socket 客户端会使用 socket()创建一个 socket，然后通过调用 connect()建立一个连接并制定服务器的众所周知的地址。当两个流 socket 连接之后就可以使用 read()和 write()在任意一个方向上传输数据了。一旦拥有引用一个流 socket 端点的文件描述符的所有进程都执行了一个隐式或显示的 close()之后，连接就会终止。

一个典型的数据报 socket 服务器会使用 socket()创建一个 socket，然后使用 bind()将其绑定到一个众所周知的地址上。由于数据报 socket 是无连接的，因此服务器的 socket 可以用来接收任意客户端的数据报。使用 read()或 socket 特定的 recvfrom()系统调用能够接收数据报，其中 recvfrom()能够返回发送 socket 的地址。一个数据报 socket 客户端会使用 socket()创建一个 socket，然后使用 sendto()将一个数据报发送到指定的（即服务器的）地址上。connect()系统调用可以用来为数据报 socket 设定一个对等地址。在设定完对等地址之后就无需为发出去的数据报指定目标地址了；write()调用可以用来发送一个数据报。
```

## chap57 SOCKET：UNIX DOMAIN

### UNIX domain socket 地址：struct sockaddr_un

```c
struct sockaddr_un{
    sa_family_t sun_family;
    char sun_path[108];
}
为将一个 UNIX domain socket 绑定到一个地址上，需要初始化一个 sockaddr_un 结构，然后将指向这个结构的一个（转换）指针作为 addr 参数传入 bind()并将 addrlen 指定为这个结构的大小。
```

![2022-06-06 21-16-08 的屏幕截图](/home/cccmmf/network programming/Linux系统编程网络部分/图片/2022-06-06 21-16-08 的屏幕截图.png)

```c
有关绑定一个 UNIX domain socket 方面还需要注意以下几点。
无法将一个 socket 绑定到一个既有路径名上（bind()会失败并返回 EADDRINUSE 错误）。
通常会将一个 socket 绑定到一个绝对路径名上，这样这个 socket 就会位于文件系统中的一个固定地址处。当然，也可以使用一个相对路径名，但这种做法并不常见，因为它要求想要 connect()这个 socket 的应用程序知道执行 bind()的应用程序的当前工作目录。
一个 socket 只能绑定到一个路径名上，相应地，一个路径名只能被一个 socket 绑定。
无法使用 open()打开一个 socket。
当不再需要一个 socket 时可以使用 unlink()（或 remove()）删除其路径名条目（通常也应该这样做）。
```

### UNIX domain socket 权限

```c
socket 文件的所有权和权限决定了哪些进程能够与这个 socket 进行通信。
要连接一个 UNIX domain 流 socket 需要在该 socket 文件上拥有写权限。
要通过一个 UNIX domain 数据报 socket 发送一个数据报需要在该 socket 文件上拥有写权限。
此外，需要在存放 socket 路径名的所有目录上都拥有执行（搜索）权限。
在默认情况下，创建 socket（通过 bind()）时会给所有者（用户）、组以及 other 用户赋予所有的权限。要改变这种行为可以在调用 bind()之前先调用 umask()来禁用不希望赋予的权限。
```

### 创建互联 socket 对：socketpair()

```c
有时候让单个进程创建一对 socket 并将它们连接起来是比较有用的。这可以通过使用两个 socket()调用和一个 bind()调用以及对 listen()、connect()、accept()（用于流 socket）的调用或对 connect()（用于数据报 socket）的调用来完成。socketpair()系统调用则为这个操作提供了一个快捷方式。

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

int socketpair(int domain, int type, int protocol, int sv[2]);
socketpair()系统调用只能用在 UNIX domain 中，即 domain 参数必须被指定为 AF_UNIX。（这个约束适用于大多数实现，但却是合理的，因为这一对 socket 是创建于单个主机系统上的。）socket 的 type 可以被指定为 SOCK_DGRAM 或 SOCK_STREAM。protocol 参数必须为 0。 sockfd数组返回了引用这两个相互连接的 socket 的文件描述符。

将 type 指定为 SOCK_STREAM 相当于创建一个双向管道（也被称为流管道）。每个 socket都可以用来读取和写入，并且这两个 socket 之间每个方向上的数据信道是分开的。（在从 BSD演化来的实现中，pipe()被实现成了一个对 socketpair()的调用。）
    
一般来讲，socket 对的使用方式与管道的使用方式类似。在调用完 socketpair()之后，进程会使用 fork()创建一个子进程。子进程会继承父进程的文件描述符的副本，包括引用 socket 对的描述符。因此父进程和子进程就可以使用这一对 socket 来进行 IPC 了。
    
使用 socketpair()创建一对 socket 与手工创建一对相互连接的 socket 这两种做法之间的一
个差别在于前一对 socket 不会被绑定到任意地址上。这样就能够避免一类安全问题了，因为
这一对 socket 对其他进程是不可见的。
    
从内核 2.6.27 开始，Linux 为 type 参数提供了第二种用途，即允许将两个非标准的标记与 socket type 取 OR 。 SOCK_CLOEXEC 标 记 会 导 致 内 核 为 两 个 新 文 件 描 述 符 启 用close-on-exec 标记（FD_CLOEXEC）。这个标记之所以有用的原因与 4.3.1 节中描述的 open()O_CLOEXEC 标记有用的原因是一样的。 SOCK_NONBLOCK 标记会导致内核在两个底层打开着的文件描述符上设置 O_NONBLOCK 标记，这样在该 socket 上发生的后续 I/O 操作就不会阻塞了。
```

### Linux 抽象 socket 名空间

```c
所谓的抽象路径名空间是 Linux 特有的一项特性，它允许将一个 UNIX domain socket 绑定到一个名字上但不会在文件系统中创建该名字。这种做法具备几点优势。
无需担心与文件系统中的既有名字产生冲突。
没有必要在使用完 socket 之后删除 socket 路径名。当 socket 被关闭之后会自动删除这个抽象名。
无需为 socket 创建一个文件系统路径名了。这对于 chroot 环境以及在不具备文件系统上的写权限时是比较有用的。
要创建一个抽象绑定就需要将 sun_path 字段的第一个字节指定为 null 字节（\0）。这样就能够将抽象 socket 名字与传统的 UNIX domain socket 路径名区分开来，因为传统的名字是由一个或多个非空字节以及一个终止 null 字节构成的字符串。sun_path 字段的余下的字节为socket 定义了抽象名字。在解释这个名字时需要用到全部字节，而不是将其看成是一个以 null结尾的字符串。
```

![2022-06-06 22-36-14 的屏幕截图](/home/cccmmf/network programming/Linux系统编程网络部分/图片/2022-06-06 22-36-14 的屏幕截图.png)

```c
使用一个初始 null 字节来区分抽象 socket 名和传统的 socket 名会带来不同寻常的结果。假设变量 name 正好指向了一个长度为零的字符串并将一个 UNIX domain socket 绑定到一个按照下列方式初始化 sun_path 的名字上。
strncpy(addr.sun_path, name, sizeof(addr.sun_path) - 1);
在 Linux 上，就会在无意中创建了一个抽象 socket 绑定。但这种代码可能并不是期望中的代码（即一个 bug）。在其他 UNIX 实现中，后续的 bind()调用会失败。
```

### 总结

```c
UNIX domain socket 允许位于同一主机上的应用程序之间进行通信。 UNIX domain 支持流和数据报 socket。
UNIX domain socket 是通过文件系统中的一个路径名来标识的。文件权限可以用来控制对UNIX domain socket 的访问。
socketpair()系统调用创建一对相互连接的 UNIX domain socket。这样就无需调用多个系统调用来创建、绑定以及连接 socket。一个 socket 对的使用方式通常与管道类似：一个进程创建socket 对，然后创建一个其引用 socket 对的描述符的子进程。然后这两个进程就能够通过这个socket 对进行通信了。
Linux 特有的抽象 socket 名空间允许将一个 UNIX domain socket 绑定到一个不存在于文件系统中的名字上。
```

## chap58 SOCKET：TCP/IP网络编程基础

### 互联网

- 互联网络（internetwork），或更一般地，互联网（internet，小写的 i），会将不同的计算机网络连接起来并允许位于网络中的主机相互之间进行通信。换句话说，一个互联网是由计算机网络组成的一个网络。术语子网络，或子网，用来指组成因特网的其中一个网络。互联网的目标是隐藏不同物理网络的细节以便向互联网络中的所有主机呈现一个统一的网络架构，例如，这意味着可以使用单个地址格式来标识互联网上的所有主机。

- 尽管已经设计出了多种互联网互联协议，但 TCP/IP 已经成了使用为最广泛的协议套件了，它甚至已经取代了之前在局域网和广域网中常见的私有联网协议了。术语 Internet（大写的 I）被用来指将全球成千上万的计算机连接起来的 TCP/IP 互联网。

### 数据链路层

- 它由设备驱动和到底层物理媒介（如电话线、同轴电缆、或光纤）的硬件接口（网卡）构成。数据链路层关注的是在一个网络的物理链接上传输数据。

- 要传输数据，数据链路层需要将网络层传递过来的数据报封装进被称为帧的一个一个单元。除了需要传输的数据之外，每个帧都会包含一个头，如头中可能包含了目标地址和帧的大小。数据链路层在物理链接上传输帧并处理来自接收者的确认。（不是所有的数据链路层都使用确认。）这一层可能会进行错误检测、重传以及流量控制。一些数据链路层还可能会将大的网络包分割成多个帧并在接收者端对这些帧进行重组。

- 对于有关 IP 的讨论来讲，数据链路层中比较重要的一个特点是最大传输单元（MTU）。数据链路层的 MTU 是该层所能传输的帧大小的上限。不同的数据链路层的 MTU 是不同的。

### 网络层：IP

- 位于数据链路层之上的是网络层，它关注的是如何将包（数据）从源主机发送到目标主机。这一层执行了很多任务，包括以下几个。

1. 将数据分解成足够小的片段以便数据链路层进行传输（如有必要的话）。
2. 在因特网上路由数据。
3. 为传输层提供服务。

#### IP 传输数据报

- IP 以数据报（包）的形式来传输数据。在两个主机之间发送的每一个数据报都是在网络上独立传输的，它们经过的路径可能会不同。一个 IP 数据报包含一个头，其大小范围为 20 字节到 60 字节。这个头中包含了目标主机的地址，这样就可以在网络上将这个数据报路由到目标地址了。此外，它还包含了包的源地址，这样接收主机就知道数据报的源头。

- 一个 IP 实现可能会给它所支持的数据报的大小设定一个上限。所有 IP 实现都必须做到数据报的大小上限至少与规定的 IP 最小重组缓冲区大小（minimum reassembly buffer size）一样大。在 IPv4 中，这个限制值是 576 字节；在 IPv6 中，这个限制值是 1500 字节。

#### IP 是无连接和不可靠的

- IP 是一种无连接协议，因为它并没有在相互连接的两个主机之间提供一个虚拟电路。IP也是一种不可靠的协议：它尽最大可能将数据报从发送者传输给接收者，但并不保证包到达的顺序会与它们被传输的顺序一致，也不保证包是否重复，甚至都不保证包是否会达到接收者。IP 也没有提供错误恢复（头信息错误的包会被静默地丢弃）。可靠性是通过使用一个可靠的传输层协议（如 TCP）或应用程序本身来保证的。
- IPv4 为 IP 头提供了一个校验和，这样就能够检测出头中的错误，但并没有为包中所传输的数据提供任何错误检测机制。IPv6 并没有为 IP 头提供检验和，它依赖高层协议来完成错误检测和可靠性。（UDP 校验和在 IPv4 是可选的，但一般来讲都是启用的；UDP 校验和在 IPv6 是强制的。TCP 校验和在 IPv4 和 IPv6 中都是强制的。）
- IP 数据报的重复是可能发生的，因为一些数据链路层采用了一些技术来确保可靠性以及 IP 数据报可能会以隧道形式穿越一些采用了重传机制的非 TCP/IP 网络。

#### IP 可能会对数据报进行分段

- IPv4 数据报的最大大小为 65 535 字节。在默认情况下，IPv6 允许一个数据报的最大大小为 65 575 字节（40 字节用于存放头信息，65 535 字节用于存放数据），并且为更大的数据报（所谓的 jumbograms）提供了一个选项。

- 之前曾经提过大多数数据链路层会为数据帧的大小设定一个上限（MTU）。如在常见的以太网架构中这个上限值是 1500 字节（比一个 IP 数据报的最大大小要小得多）。IP 还定义了路径 MTU 的概念，它是源主机到目的主机之间路由上的所有数据链路层的最小 MTU。（在实践中，以太网 MTU 通常是路径中最小的 MTU。）

- 当一个 IP 数据报的大小大于 MTU 时，IP 会将数据报分段（分解）成一个个大小适合在网络上传输的单元。这些分段在达到最终目的地之后会被重组成原始的数据报。（每个 IP 分段本身就是包含了一个偏移量字段的 IP 数据报，该字段给出了一个该分段在原始数据报中的位置。）

- IP 分段的发生对于高层协议层是透明的，并且一般来讲也并不希望发生这种事情（[Kent& Mogul, 1987]）。这里的问题在于由于 IP 并不进行重传并且只有在所有分段都达到目的地之后才能对数据报进行组装，因此如果其中一些分段丢失或包含传输错误的话就会导致整个数据报不可用。在一些情况下，这会导致极高的数据丢失率（适用于不进行重传的高层协议，如 UDP）或降低传输速率（适用于进行重传的高层协议，如 TCP）。现代 TCP 实现采用了一些算法（路径 MTU 发现）来确定主机之间的一条路径的 MTU，并根据该值对传递给 IP 的数据进行分解，这样 IP 就不会碰到需要传输大小超过 MTU 的数据报的情况了。UDP 并没有提供这种机制，在58.6.2 节中将会考虑基于 UDP 的应用程序如何处理 IP 分段的情况。

### IP 地址

#### IPv4地址

- 一个 IP 地址包含两个部分：一个是网络 ID，它指定了主机所属的网络；另一个是主机 ID，它标识出了位于该网络中的主机。

![image.2C4VM1](/tmp/evince-4647/image.2C4VM1.png)

- 当一个组织为其主机申请一组 IPv4 地址时，它会收到一个 32 位的网络地址以及一个对应的 32 位的网络掩码。在二进制形式中，这个掩码最左边的位由 1 构成，掩码中剩余的位用 0填充。这些 1 表示地址中哪些部分包含了所分配到的网络 ID，而这些 0 则表示地址中哪些部分可供组织用来为网络中的主机分配唯一的 ID。掩码中网络 ID 部分的大小会在分配地址时确定。由于网络 ID 部分总是占据着掩码最左边的部分，因此可以通过下面的标记法来指定分配的地址范围。

​		204.152.189.0/24

- 这里的/24 表示分配的地址的网络 ID 由最左边的 24 位构成，剩余的 8 位用于指定主机 ID。或者在这种情况下也可以说网络掩码的点分十进制标记是 255.255.255.0。

- 拥有这个地址的组织可以将 254 个唯一的因特网地址分配给其计算机——204.152.189.1 204.152.189.254。有两个地址是无法分配给计算机的，其中一个地址的主机 ID 的位都是 0，它用来标识网络本身，另一个地址的主机 ID 的位都是 1——在本例中是 204.152.189.255——它是子网广播地址。

- 一些 IPv4 地址拥有特殊的含义。特殊地址 127.0.0.1 一般被定义为回环地址，它通常会被分配给主机名 localhost。（网络 127.0.0.0/8 中的所有地址都可以被指定为 IPv4 回环地址，但通常会选择 127.0.0.1。）发送到这个地址的数据报实际上不会到达网络，它会自动回环变成发送主机的输入。使用这个地址可以便捷地在同一主机上测试客户端和服务器程序。在 C 程序中定义了整数常量 INADDR_LOOPBACK 来表示这个程序。

- 常量 INADDR_ANY 就是所谓的 IPv4 通配地址。通配 IP 地址对于将 Internet domainsocket 绑定到多宿主机上的应用程序来讲是比较有用的。如果位于一台多宿主机上的应用程序只将 socket 绑定到其中一个主机 IP 地址上，那么该 socket 就只能接收发送到该 IP 地址上的 UDP 数据报和 TCP 连接请求。但一般来讲都希望位于一台多宿主机上的应用程序能够接收指定任意一个主机 IP 地址的数据报和连接请求，而将 socket 绑定到通配 IP 地址上使之成为了可能。 SUSv3 并没有为 INADDR_ANY 规定一个特定的值，但大多数实现将其定义成了
  0.0.0.0（全是 0）。

- 一般来讲，IPv4 地址是划分子网的。划分子网将一个 IPv4 地址的主机 ID 部分分成两个部分：一个子网 ID 和一个主机 ID（图 58-6）。（如何划分主机 ID 的位完全是由网络管理员来决定的。）子网划分的原理在于一个组织通常不会将其所有主机接到单个网络中。相反，组织可能会开启一组子网（一个“内部互联网络”），每个子网使用网络 ID 和子网 ID 组合起来标识。这种组合通常被称为扩展网络 ID。在一个子网中，子网掩码所扮演的角色与之前描述的网络掩码的角色是一样的，并且可以使用类似的标记法来表示分配给一个特定子网的地址范围。

- 例如假设分配到的网络 ID 是 204.152.189.0/24，这样可以通过将主机 ID 的 8 位中的 4 位划分成子网 ID 并将剩余的 4 位划分成主机 ID 来对这个地址范围划分子网。在这种情况下，子网掩码将由 28 个前导 1 后面跟着 4 个 0 构成， ID 为 1 的子网将会被表示为 204.152.189.16/28。

  

  ![image.HD9DN1](/tmp/evince-4647/image.HD9DN1.png)



#### IPv6 地址

- IPv6 地址的原理与 IPv4 地址是类似的，它们之间关键的差别在于 IPv6 地址由 128 位构成，其中地址中的前面一些位是一个格式前缀，表示地址类型。

- IPv6 地址通常被书写成一系列用冒号隔开的 16 位的十六进制数字，如下所示。

- F000:0:0:0:0:0:A:1

- IPv6 地址通常包含一个 0 序列，并且为了标记方便，可以使用两个分号（::）来表示这种序列。因此上面的地址可以被重写成：

- F000:A:1

- 在 IPv6 地址中只能出现一个双冒号标记，出现多次的话会造成混淆。

- IPv6 也像 IPv4 地址那样提供了环回地址（127 个 0 后面跟着一个 1，即::1）和通配地址（所有都为 0，可以书写成 0::0 或::）。

- 为允许 IPv6 应用程序与只支持 IPv4 的主机进行通信， IPv6 提供了所谓的 IPv4 映射的 IPv6地址，图 58-7 给出了这些地址的格式。

  

  ![image.F0FXM1](/tmp/evince-4647/image.F0FXM1.png)

- 在书写 IPv4 映射的 IPv6 地址时，地址的 IPv4 部分（即最后 4 个字节）会被书写成 IPv4的 点 分 十 进 制 标 记 。 因 此 与 204.152.189.116 等 价 的 IPv4 映 射 的 IPv6 地 址是::FFFF:204.152.189.116。

### 传输层

- 在 TCP/IP 套件中使用广泛的两个传输层协议如下。

1. 用户数据报协议（UDP）是数据报 socket 所使用的协议。
2. 传输控制协议（TCP）是流 socket 所使用的协议。

#### 端口号

- 传输层协议的任务是向位于不同主机（或有时候位于同一主机）上的应用程序提供端到端的通信服务。为完成这个任务，传输层需要采用一种方法来区分一个主机上的应用程序。在 TCP 和 UDP 中，这种区分工作是通过一个 16 位的端口号来完成的。

##### 众所周知的、注册的以及特权端口

- 有些众所周知的端口号已经被永久地分配给特定的应用程序了（也称为服务）。例如 ssh（安全的 shell）daemon 使用众所周知的端口 22，HTTP（Web 服务器和浏览器之间通信时所采用的协议）使用众所周知的端口 80。众所周知的端口的端口号位于 0～1023 之间，它是由中央授权机构互联网号码分配局（IANA, http://www.iana.org/）来分配的。一个众所周知的端口号的分配是由一个被核准的网络规范（通常以 RFC 的形式）来规定的。

- IANA 还记录着注册端口，将这些端口分配给应用程序开发人员的过程就不那么严格了（这也意味着一个实现无需保证这些端口是否真正用于它们注册时申请的用途）。 IANA 注册的端口范围为 1024～41951。（不是所有位于这个范围内的端口都被注册了。）

- 大多数 TCP/IP 实现（包括 Linux）中，范围在 0 到 1023 间的端口号也是特权端口，这意味着只有特权（CAP_NET_BIND_SERVICE）进程可以绑定到这些端口上，从而防止了普通用户通过实现恶意程序（如伪造 ssh）来获取密码。（有些时候，特权端口也被称为保留端口。）
- 尽管端口号相同的 TCP 和 UDP 端口是不同的实体，但同一个众所周知的端口号通常会同时被分配给基于 TCP 和 UDP 的服务，即使该服务通常只提供了其中一种协议服务。这种惯例避免了端口号在两个协议中产生混淆的情况。

##### 临时端口

- 如果一个应用程序没有选择一个特定的端口（即在 socket 术语中，它没有调用 bind()将其 socket 绑定到一个特定的端口上），那么 TCP 和 UDP 会为该 socket 分配一个唯一的临时端口（即存活时间较短）。在这种情况下，应用程序—通常是一个客户端—并不关心它所使用的端口号，但分配一个端口对于传输层协议标识通信端点来讲是有必要的。这种做法的另一个结果是位于通信信道另一端的对等应用程序就知道如何与这个应用程序通信了。TCP 和 UDP 在将 socket 绑定到端口 0 上时也会分配一个临时端口号。
- IANA 将位于 49152 到 65535 之间的端口称为动态或私有端口，这表示这些端口可供本地应用程序使用或作为临时端口分配。然后不同的实现可能会在不同的范围内分配临时端口。在 Linux 上，这个范围是由包含在文件/proc/sys/net/ipv4/ip_local_port_range 中的两个数字来定义的（可通过修改这两个数字来修改范围）。

#### 用户数据报协议（UDP）

- UDP 仅仅在 IP 之上添加了两个特性：端口号和一个进行检测传输数据错误的数据校验和。
- 与 IP 一样，UDP 也是无连接的。由于它并没有在 IP 之上增加可靠性，因此 UDP 是不可靠的。如果一个基于 UDP 的应用程序需要确保可靠性，那么这项功能就必须要在应用程序中予以实现。
- UDP 和 TCP 使用的校验和的长度只有 16 位并且只是简单的“总结性”校验和，因此无法检测出特定的错误，其结果是无法提供较强的错误检测机制。繁忙的互联网服务器通常只能每隔几天看一下未检测出的传输错误的平均情况（[Stone & Partridge, 2000]）。需要更多确保数据完整性的应用程序可以使用安全 Sockets 层（Secure Sockets Layer，SSL），它不仅仅提供了安全的通信，而且还提供更加严格的错误检测过程。或者应用程序也可以实现自己的错误控制机制。

##### 选择一个 UDP 数据报大小以避免 IP 分段

- 在 58.4 节中描述过 IP 分段机制并指出过通常应该尽可能地避免 IP 分段。TCP 提供了避免 IP 分段的机制，但 UDP 并没有提供相应的机制。使用 UDP 时如果传输的数据报的大小超过了本地数据链接的 MTU，那么很容易就会导致 IP 分段。
- 基于 UDP 的应用程序通常不会知道源主机和目的主机之间的路径的 MTU。一般来讲，基于 UDP 的应用程序会采用保守的方法来避免 IP 分段，即确保传输的 IP 数据报的大小小于IPv4 的组装缓冲区大小的最小值 576 字节。（这个值很有可能是小于路径 MTU 的。）在这 576字节中，有 8 个字节是用于存放 UDP 头的，另外最少需要使用 20 个字节来存放 IP 头，剩下的 548 字节用于存放 UDP 数据报本身。在实践中，很多基于 UDP 的应用程序会选择使用一个更小的值 512 字节来存放数据报。

#### 传输控制协议（TCP）

- TCP 在两个端点（即应用程序）之间提供了可靠的、面向连接的、双向字节流通信信道。



![image.UR1XM1](/tmp/evince-4647/image.UR1XM1.png)



- 这里使用术语 TCP 端点来表示 TCP 连接一端的内核所维护的信息。（通常会进一步对这个术语进行缩写，如仅书写“一个 TCP”来表示“一个 TCP 端点”或“客户端 TCP”来表示“客户端应用程序维护的 TCP 端点。”）这部分信息包括连接这一端的发送和接收缓冲区以及维护的用来同步两个已连接的端点的操作的状态信息。（在 61.6.3 节中介绍 TCP 状态迁移图时将深入介绍状态信息的细节。）在本书余下的部分中将使用术语接收 TCP 和发送 TCP 来表示一个用来在特定方向上传输数据的流 socket 连接两端的接收和发送应用程序。

##### 连接建立

- 在开始通信之前，TCP 需要在两个端点之间建立一个通信信道。在连接建立期间，发送者和接收者需要交换选项来协商通信的参数。

##### 将数据打包成段

- 数据会被分解成段，每一个段都包含一个校验和，从而能够检测出端到端的传输错误。
  每一个段使用单个 IP 数据报来传输。

##### 确认、重传以及超时

- 当一个 TCP 段无错地达到目的地时，接收 TCP 会向发送者发送一个确认，通知它数据发送递送成功了。如果一个段在到达时是存在错误的，那么这个段就会被丢弃，确认信息也不会被发送。为处理段永远不到达或被丢弃的情况，发送者在发送每一个段时会开启一个定时器。如果在定时器超时之前没有收到确认，那么就会重传这个段。

- 由于所使用的网络以及当前的流量负载会影响传输一个段和接收其确认所需的时间，因此 TCP 采用了一个算法来动态地调整重传超时时间（RTO）的大小。
- 接收 TCP 可能不会立即发送确认，而是会等待几毫秒来观察一下是否可以将确认塞进接收者返回给发送者的响应中。（每个 TCP 段都包含一个确认字段，这样就能将确认塞进TCP 段中了。）这项被称为延迟 ACK 的技术的目的是能少发送一个 TCP 段，从而降低网络中包的数量以及降低发送和接收主机的负载。

##### 排序

- 在 TCP 连接上传输的每一个字节都会分配到一个逻辑序号。这个数字指出了该字节在这个连接的数据流中所处的位置。（这个连接中的两个流各自都有自己的序号计数系统。）当传输一个 TCP 分段时会在其中一个字段中包含这个段的第一个字节的序号。

- 在每一个段中加上一个序号有几个作用。

1. 这个序号使得 TCP 分段能够以正确的顺序在目的地进行组装，然后以字节流的形式传递给应用层。（在任意一个时刻，在发送者和接收者之间可能存在多个正在传输的 TCP分段，这些分段的到达顺序可能与被发送的顺序可能是不同的。）
2. 由接收者返回给发送者的确认消息可以使用序号来标识出收到了哪个 TCP 分段。
3. 接收者可以使用序号来移除重复的分段。发生重复的原因可能是因为 IP 数据段重复，也可能是因为 TCP 自己的重传算法会在一个段的确认丢失或没有按时收到时重传一个成功递送出去的段。

- 一个流的初始序号（ISN）不是从 0 开始的，相反，它是通过一个算法来生成的，该算法会递增分配给后续 TCP 连接的 ISN（为防止出现前一个连接中的分段与这个连接中的分段混淆的情况）。这个算法也使得猜测 ISN 变得困难起来。序号是一个 32 位的值，当到达最大取值时会回到 0。

##### 流量控制

- 流量控制防止一个快速的发送者将一个慢速的接收者压垮。要实现流量控制，接收 TCP就必须要为进入的数据维护一个缓冲区。（每个 TCP 在连接建立阶段会通告其缓冲区的大小。）当从发送 TCP 端收到数据时会将数据累积在这个缓冲区中，当应用程序读取数据时会从缓冲区中删除数据。在每个确认中，接收者会通知发送者其进入数据缓冲区的可用空间（即发送者可以发送多少字节）。 TCP 流量控制算法采用了所谓的滑动窗口算法，它允许包含总共 N 字节（提供的窗口大小）的未确认段同时在发送者和接收者之间传输。如果接收 TCP 的进入数据缓冲区完全被充满了，那么窗口就会关闭，发送 TCP 就会停止传输数据。

##### 拥塞控制：慢启动和拥塞避免算法

- TCP 的拥塞控制算法被设计用来防止快速的发送者压垮整个网络。如果一个发送 TCP 发送包的速度要快于一个中间路由器转发的速度，那么该路由器就会开始丢弃包。这将会导致较高的包丢失率，其结果是如果 TCP 保持以相同的速度发送这些被丢弃的分段的话就会极大地降低性能。TCP 的拥塞控制算法在下列两个场景中是比较重要的。

1. 在连接建立之后：此时（或当传输在一个已经空闲了一段时间的连接上恢复时），发送者可以立即向网络中注入尽可能多的分段，只要接收者公告的窗口大小允许即可。（事实上，这就是早期的 TCP 实现的做法。）这里的问题在于如果网络无法处理这种分段洪泛，那么发送者会存在立即压垮整个网络的风险。
2. 当拥塞被检测到时：如果发送 TCP 检测到发生了拥塞，那么它就必须要降低其传输速率。TCP 是根据分段丢失来检测是否发生了拥塞，因为传输错误率是非常低的，即如果一个包丢失了，那么就认为发生了拥塞。

- TCP 的拥塞控制策略组合采用了两种算法：慢启动和拥塞避免。
- 慢启动算法会使发送 TCP 在一开始的时候以低速传输分段，但同时允许它以指数级的速度提高其速率，只要这些分段都得到接收 TCP 的确认。慢启动能够防止一个快速的 TCP 发送者压垮整个网络。但如果不加限制的话，慢启动在传输速率上的指数级增长意味着发送者在短时间内就会压垮整个网络。TCP 的拥塞避免算法用来防止这种情况的发生，它为速率的增长安排了一个管理实体。
- 有了拥塞避免之后，在连接刚建立时，发送 TCP 会使用一个较小的拥塞窗口，它会限制所能传输的未确认的数据数量。当发送者从对等 TCP 处接收到确认时，拥塞窗口在一开始时会呈现指数级增长。但一旦拥塞窗口增长到一个被认为是接近网络传输容量的阈值时，其增长速度就会变成线性，而不是指数级的。（对网络容量的估算是根据检测到拥塞时的传输速率来计算得出的或者在一开始建立连接时设定为一个固定值。）在任何时刻，发送 TCP 传输的数据数量还会受到接收 TCP 的通告窗口和本地的 TCP 发送缓冲器的大小的限制。
- 慢启动和拥塞避免算法组合起来使得发送者可以快速地将传输速度提升至网络的可用容量，并且不会超出该容量。这些算法的作用是允许数据传输快速地到达一个平衡状态，即发送者传输包的速率与它从接收者处接收确认的速率一致。

### 总结

- TCP/IP 是一个分层的联网协议条件。在 TCP/IP 协议栈的最底层是 IP 网络层协议。IP 以数据报的形式传输数据。IP 是无连接的，表示在源主机和目的主机之间传输的数据报可能经过网络中的不同路径。IP 是不可靠的，因为它不保证数据报会按序以及不重复到达，甚至还不保证数据报一定会到达。如果要求可靠性的话就必须要通过使用一个可靠的高层协议（如TCP）或在应用程序中来完成。
- IP 最初的版本是 IPv4。在 20 世纪 90 年代早期， IP 的一个新版本 IPv6 被设计出来了。 IPv4和 IPv6 之间最显著的差别在于 IPv4 使用了 32 位来表示一个主机地址，而 IPv6 则使用了 128位，从而允许在全球范围的因特网中接入更多的主机。目前，IPv4 仍然是使用最为广泛的 IP，尽管在将来可能会被 IPv6 所取代。
- 在 IP 之上存在多种传输层协议，其中使用最多的是 UDP 和 TCP。UDP 是一个不可靠的数据报协议。TCP 是一个可靠的、面向连接的字节流协议。TCP 处理了连接建立和终止的所有细节。TCP 还将数据打包成分段以供 IP 传输并为这些分段提供了序号计数，这样接收者就能对这些分段进行确认并以正确的顺序组装这些分段。此外，TCP 还提供了流量控制来防止一个快速的发送者压垮一个慢速的接收者和拥塞控制来防止一个快速的发送者压垮整个网络。

## chap59 SOCKET：Internet Domain

- 在第 58 章中提到过 Internet domain socket 地址由一个 IP 地址和一个端口号组成。虽然计算机使用了 IP 地址和端口号的二进制表示形式，但人们对名称的处理能力要比对数字的处理能力强得多。因此，本章将介绍使用名称标识主机计算机和端口的技术。此外，还将介绍如何使用库函数来获取特定主机名的 IP 地址和与特定服务名对应的端口号，其中对主机名的讨论还包括了对域名系统（DNS）的描述，域名系统是一个分布式数据库，它将主机名映射到IP 地址以及将 IP 地址映射到主机名。

### Internet domain socket

- Internet domain 流 socket 是基于 TCP 之上的，它们提供了可靠的双向字节流通信信道。
- Internet domain 数据报 socket 是基于 UDP 之上的。UDP socket 与之在 UNIX domain 中的对应实体类似，但需要注意下列差别。

1. UNIX domain 数据报 socket 是可靠的，但 UDP socket 则是不可靠的—数据报可能会丢失、重复或到达的顺序与它们被发送的顺序不同。
2. 在一个 UNIX domain 数据报 socket 上发送数据会在接收 socket 的数据队列为满时阻塞。与之不同的是，使用 UDP 时如果进入的数据报会使接收者的队列溢出，那么数据报就会静默地被丢弃。

### 网络字节序

- 由于端口号和 IP 地址必须在网络中的所有主机之间传递并且需要被它们所理解，因此必须要使用一个标准的字节序。这种字节序被称为网络字节序，它是大端（高存低，低存高）的。

- 在本章后面将会介绍各种用于将主机名（如 www.kernel.org）和服务名（如 http）转换成对应的数字形式的函数。这些函数一般会返回用网络字节序表示的整数，并且可以直接将这些整数复制进一个 socket 地址结构的相关字段中。
- 有时候可能会直接使用 IP 地址和端口号的整数常量形式，如可能会选择将端口号硬编码进程序中，或者将端口号作为一个命令行参数传递给程序，或者在指定一个 IPv4 地址时使用诸如INADDR_ANY 和 INADDR_LOOPBACK 之类的常量。这些值在 C 中是按照主机的规则来表示的，因此它们是主机字节序的，在将它们存储进 socket 地址结构中之前需要将这些值转换成网络字节序。
- htons()、htonl()、ntohs()以及 ntohl()函数被定义（通常为宏）用来在主机和网络字节序之间转换整数。

```c
#include <arpa/inet.h>

uint32_t htonl(uint32_t hostlong);

uint16_t htons(uint16_t hostshort);

uint32_t ntohl(uint32_t netlong);

uint16_t ntohs(uint16_t netshort);
```

### 数据表示

- 在编写网络程序时需要清楚不同的计算机架构使用不同的规则来表示各种数据类型。本章之前已经指出过整数类型可以以大端或小端的形式存储。此外，还存在其他的差别，如 C long数据类型在一些系统中可能是 32 位的，但在其他系统上可能是 64 位的。当考虑结构时，问题就更加复杂了，因为不同的实现采用了不同的规则来将一个结构中的字段对齐到主机系统的地址边界，从而使得字段之间的填充字节数量是不同的。

- 由于在数据表现上存在这些差异，因此在网络中的异构系统之间交换数据的应用程序必须要采用一些公共规则来编码数据。发送者必须要根据这些规则来对数据进行编码，而接收者则必须要遵循同样的规则对数据进行解码。将数据变成一个标准格式以便在网络上传输的过程被称为信号编集（marshalling）。目前，存在多种信号编集标准，如 XDR（ExterExternalData Representation，在 RFC 1014 中描述）、ASN.1-BER（Abstract SyntaxNotation 1, http://www.asn1.org/）、CORBA以及 XML。一般来讲，这些标准会为每一种数据类型都定义一个固定的格式（如定义了字节序和使用的位数）。除了按照所需的格式进行编码之外，每一个数据项都需要使用额外的字段来标识其类型（以及可能的话还会加上长度）。

- 然而，一种比信号编集更简单的方法通常会被采用：将所有传输的数据编码成文本形式，其中数据项之间使用特定的字符来分隔开，这个特定的字符通常是换行符。这种方法的一个优点是可以使用 telnet 来调试一个应用程序。要完成这项任务需要使用下面的命令。
- $talnet host port

### Internet socket地址

#### IPv4 socket 地址：struct sockaddr_in

- 个 IPv4 socket 地址会被存储在一个 sockaddr_in 结构中，该结构在<netinet/in.h>中进行定义，具体如下。

```c
 struct sockaddr_in {
              sa_family_t sin_family; /* 地址族: AF_INET */
              u_int16_t sin_port; /* 按网络字节次序的端口 */
              struct in_addr sin_addr; /* internet地址 */
     `		  unsigned char __pad[x];
              };

              /* Internet地址. */
              struct in_addr {
              u_int32_t s_addr; /* 按网络字节次序的地址 */
              };
```

#### IPv6 socket 地址：struct sockaddr_in6

- 与 IPv4 地址一样，一个 IPv6 socket 地址包含一个 IP 地址和一个端口号，它们之间的差别在于 IPv6 地址是 128 位而不是 32 位的。一个 IPv6 socket 地址会被存储在一个 sockaddr_in6结构中，该结构在<netinet/in.h>中进行定义，具体如下。

```c
struct sockaddr_in6 {
     sa_family_t sin6_family;    /* AF_INET6 */
     in_port_t sin6_port;        /* Transport layer port # */
     uint32_t sin6_flowinfo;     /* IPv6 flow information */
     struct in6_addr sin6_addr;  /* IPv6 address */
     uint32_t sin6_scope_id;     /* IPv6 scope-id */
 };
 struct in6_addr {
     union {
         uint8_t u6_addr8[16];
         uint16_t u6_addr16[8];
         uint32_t u6_addr32[4];
     } in6_u;
 
     #define s6_addr                 in6_u.u6_addr8
     #define s6_addr16               in6_u.u6_addr16
     #define s6_addr32               in6_u.u6_addr32
 };
```

- sin_family 字段会被设置成 AF_INET6。sin6_port 和 sin6_addr 字段分别是端口号和 IP地址。（uint8_t 数据类型被用来定义 in6_addr 结构中字节的类型，它是一个 8 位的无符号整型。）剩余的字段 sin6_flowinfo 和 sin6_scope_id 则超出了本书的范围，在本书给出所有例子中都会将它们设置为 0。sockaddr_in6 结构中的所有字段都是以网络字节序存储的。

- IPv6 和 IPv4 一样也有通配和回环地址，但它们的用法要更加复杂一些，因为 IPv6 地址是存储在数组中的（并没有使用标量类型），下面将会使用 IPv6 通配地址（0::0）来说明这一点。系统定义了常量 IN6ADDR_ANY_INIT 来表示这个地址，具体如下。
- #define IN6ADDR_ANY {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}
- Linux 上，头文件中的一些细节与本节中的描述是不同的。特别地，in6_addr 结构包含了一个 union 定义将 128 位的 IPv6 地址划分成 16 字节或八个 2 字节的整数或四个 32 字节的整数。由于存在这样的定义，因此 glibc 提供的 IN6ADDR_ANY_INIT 常量的定义实际上比正文中给出的定义多了一组嵌套的花括号。

- 在变量声明的初始化器中可以使用 IN6ADDR_ANY_INIT 常量，但无法在一个赋值语句的右边使用这个常量，因为 C 语法并不允许在赋值语句中使用一个结构化的常量。取而代之的做法是必须要使用一个预先定义的变量 in6addr_any，C 库会按照下面的方式对该变量进行初始化。
- const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
- 因此可以像下面这样使用通配地址来初始化一个 IPv6 socket 地址。

```c
struct sockaddr_in6 addr;

memset(&addr, 0, sizeof(addr));
addr.sin6_family = AF_INET6;
addr.sin6_addr = in6addr_any;
addr.sin6_port = htons(SOME_PORT_NUM);
```

- IPv6 环回地址（::1）的对应常量和变量是 IN6ADDR_LOOPBACK_INIT 和 in6addr_loopback。

- 与 IPv4 中相应字段不同的是 IPv6 的常量和变量初始化器是网络字节序的，但就像上面给出的代码那样，开发人员仍然必须要确保端口号是网络字节序的。

- 如果 IPv4 和 IPv6 共存于一台主机上，那么它们将共享同一个端口号空间。这意味着如果一个应用程序将一个 IPv6 socket 绑定到了 TCP 端口 2000 上（使用 IPv6 通配地址），那么 IPv4 TCP socket 将无法绑定到同一个端口上。（TCP/IP 实现确保位于其他主机上的socket 能够与这个 socket 进行通信，不管那些主机运行的是 IPv4 还是 IPv6。）

#### sockaddr_storage 结构

- 在 IPv6 socket API 中新引入了一个通用的 sockaddr_storage 结构，这个结构的空间足以存储任意类型的 socket 地址（即可以将任意类型的 socket 地址结构强制转换并存储在这个结构中）。特别地，这个结构允许透明地存储 IPv4 或 IPv6 socket 地址，从而删除了代码中的 IP 版本依赖性。sockaddr_storage 结构在 Linux 上的定义如下所示。

```c
 #if ULONG_MAX > 0xffffffff
 #define __ss_aligntype __uint64_t
 #else
 #define __ss_aligntype __uint32_t
 #endif
 #define _SS_SIZE        128
 #define _SS_PADSIZE     (_SS_SIZE - (2 * sizeof (__ss_aligntype)))
 
 struct sockaddr_storage
 {
     sa_family_t ss_family;      /* Address family */
     __ss_aligntype __ss_align;  /* Force desired alignment.  */
     char __ss_padding[_SS_PADSIZE];
 };
```

### 主机和服务转换函数概述

- 计算机以二进制形式来表示 IP 地址和端口号，但人们发现名字比数字更容易记忆。使用符号名还能有效地利用间接关系，用户和程序可以继续使用同一个名字，即使底层的数字值发生了变化也不会受到影响。
- 主机名和连接在网络上的一个系统（可能拥有多个 IP 地址）的符号标识符。服务名是端口号的符号表示。
- 主机地址和端口的表示有下列两种方法。

1. 主机地址可以表示为一个二进制值或一个符号主机名或展现格式（IPv4 是点分十进制，IPv6 是十六进制字符串）。
2. 端口号可以表示为一个二进制值或一个符号服务名。

- 格式之间的转换工作可以通过各种库函数来完成。本节将对这些函数进行简要的小结。下面几个小节将会详细描述现代 API（inet_ntop()、inet_pton()、getaddrinfo()、getnameinfo()等）。在 59.13 节中将会简要地讨论一下被废弃的 API（inet_aton()、inet_ntoa()、gethostbyname()、getservbyname()等）。

#### 在二进制和人类可读的形式之间转换 IPv4 地址

- inet_aton()和 inet_ntoa()函数将一个 IPv4 地址在点分十进制表示形式和二进制表示形式之间进行转换。这里介绍这些函数的主要原因是读者在遗留代码中可能会看到这些函数。现在它们已经被废弃了。需要完成此类转换工作的现代程序应该使用接下来描述的函数。

#### 在二进制和人类可读的形式之间转换 IPv4 和 IPv6 地址

- net_pton()和 inet_ntop()与 inet_aton()和 inet_ntoa()类似，但它们还能处理 IPv6 地址。它们将二进制 IPv4 和 IPv6 地址转换成展现格式—即以点分十进制表示或十六进制字符串表示，或将展现格式转换成二进制 IPv4 和 IPv6 地址。
- 由于人类对名字的处理能力要比对数字的处理能力强，因此通常偶尔才会在程序中使用这些函数。inet_ntop()的一个用途是产生 IP 地址的一个可打印的表示形式以便记录日志。在有些情况下，最好使用这个函数而不是将一个 IP 地址转换（“解析”）成主机名，其原因如下。

1. 将一个 IP 地址解析成主机名可能需要向一台 DNS 服务器发送一个耗时较长的请求。
2. 在一些场景中，可能并不存在一个 DNS（PTR）记录将 IP 地址映射到对应的主机名上。

#### 主机和服务名与二进制形式之间的转换（已过时）

- gethostbyname()函数返回与主机名对应的二进制 IP 地址， getservbyname()函数返回与服务名对应的端口号。对应的逆向转换是由 gethostbyaddr()和 getservbyport()来完成的。这里之所以要介绍这些函数是因为它们在既有代码中被广泛使用，但现在它们已经过时了。（SUSv3将这些函数标记为过时的，SUSv4 删除了它们的规范。）新代码应该使用 getaddrinfo()和getnameinfo()函数（稍后介绍）来完成此类转换。

#### 主机和服务名与二进制形式之间的转换（现代的）

- getaddrinfo()函数是 gethostbyname()和 getservbyname()两个函数的现代继任者。给定一个主机名getaddrinfo()会返回一组包含对应的二进制 IP 地址和端口号的结构。和一个服务名，与 gethostbyname()
  不同， getaddrinfo()会透明地处理 IPv4 和 IPv6 地址。因此使用这个函数可以编写不依赖于 IP 版本的程序。所有新代码都应该使用 getaddrinfo()来将主机名和服务名转换成二进制表示。
- getnameinfo()函数执行逆向转换，即将一个 IP 地址和端口号转换成对应的主机名和服务名。
- 使用 getaddrinfo()和 getnameinfo()还可以在二进制 IP 地址与其展现格式之间进行转换。
- DNS 允许协作服务器维护一个将二进制 IP 地址映射到主机名和将主机名映射到二进制 IP 地址的分布式数据库。诸如 DNS 之类的系统的存在对于因特网的运转是非常关键的，因为对浩瀚的因特网主机名进行集中管理是不可能的。/etc/services 文件将端口号映射到符号服务名。

### inet_pton()和 inet_ntop()函数

- inet_pton()和 inet_ntop()函数允许在 IPv4 和 IPv6 地址的二进制形式和点分十进制表示法或十六进制字符串表示法之间进行转换。

```c
#include <arpa/inet.h>

int inet_pton(int af, const char *src, void *dst);
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
```

- 这些函数名中的 p 表示“展现（presentation）”，n 表示“网络（network）”。展现形式是
  人类可读的字符串，如：

1. 204.152.189.116（IPv4 点分十进制地址）；
2. ::1（IPv6 冒号分隔的十六进制地址）；
3. ::FFFF:204.152.189.116（IPv4 映射的 IPv6 地址）。

- inet_pton()函数将 src_str 中包含的展现字符串转换成网络字节序的二进制 IP 地址。 domain 参数应该被指定为 AF_INET 或 AF_INET6。转换得到的地址会被放在 addrptr 指向的结构中，它应该根据在 domain 参数中指定的值指向一个 in_addr 或 in6_addr 结构。

- inet_ntop()函数执行逆向转换。同样， domain 应该被指定为 AF_INET 或 AF_INET6，addrptr 应该指向一个待转换的 in_addr 或 in6_addr 结构。得到的以 null 结尾的字符串会被放置在 dst_str 指向的缓冲器中。len 参数必须被指定为这个缓冲器的大小。inet_ntop()在成功时会返回 dst_str。如果 len 的值太小了，那么 inet_ntop()会返回 NULL 并将 errno设置成 ENOSPC。

- 要正确计算 dst_str 指向的缓冲器的大小可以使用在<netinet/in.h>中定义的两个常量。这些常量标识出了 IPv4 和 IPv6 地址的展现字符串的最大长度（包括结尾的 null 字节）。
- #define INET_ADDRSTRLEN 16
- #define INET6_ADDRSTRLEN 46

### 域名系统（DNS）

- 在 59.10 节中将会介绍获取与一个主机名对应的 IP 地址的 getaddrinfo()函数和执行逆向转换的 getnameinfo()函数，但在介绍这些函数之前需要解释如何使用 DNS 来维护主机名和 IP地址之间的映射关系。
- gethostbyname()函数（被 getaddrinfo()取代的函数）通过搜索这个文件并找出与规范主机名（即主机的官方或主要名称）或其中一个别名（可选的，以空格分隔）匹配的记录来获取一个IP 地址。
- DNS 的关键想法如下。

1. 将主机名组织在一个层级名空间中（图 59-2）。DNS 层级中的每一个节点都有一个标签（名字），该标签最多可包含 63 个字符。层级的根是一个无名子的节点，即“匿名节点”。
2. 一个节点的域名由该节点到根节点的路径中所有节点的名字连接而成，各个名字之间用点（.）分隔。如 google.com 是节点 google 的域名。
3. 完全限定域名（fully qualified domain name，FQDN），如 www.kernel.org.，标识出了层级中的一台主机。区分一个完全限定域名的方法是看名字是否已点结尾，但在很多情况下这个点会被省略。
4. 没有一个组织或系统会管理整个层级。相反，存在一个 DNS 服务器层级，每台服务器管理树的一个分支（一个区域）。通常，每个区域都有一个主要主名字服务器。此外，还包含一个或多个从名字服务器（有时候也被称为次要主名字服务器），它们在主要主名字服务器崩溃时提供备份。区域本身可以被划分成一个个单独管理的更小的区域。当一台主机被添加到一个区域中或主机名到 IP 地址之间的映射关系发生变化时，管理员负责更新本地名字服务器上的名字数据中的对应名字。（无需手动更改层级中其他名字服务器数据库）。
5. 当一个程序调用 getaddrinfo()来解析（即获取 IP 地址）一个域名时，getaddrinfo()会使用一组库函数（resolver 库）来与本地的 DNS 服务器通信。如果这个服务器无法提供所需的信息，那么它就会与位于层级中的其他 DNS 服务器进行通信以便获取信息。有时候，这个解析过程可能会花费很多时间，DNS 服务器采用了缓存技术来避免在查询常见域名时所发生的不必要的通信。

- 使用上面的方法使得 DNS 能够处理大规模的名空间，同时无需对名字进行集中管理。

#### 递归和迭代的解析请求

- DNS 解析请求可以分为两类：递归和迭代。在一个递归请求中，请求者要求服务器处理整个解析任务，包括在必要的时候与其他 DNS 服务器进行通信的任务。当位于本地主机上的一个应用程序调用 getaddrinfo()时，该函数会向本地 DNS 服务器发起一个递归请求。如果本地 DNS 服务器自己并没有相关信息来完成解析，那么它就会迭代地解析这个域名。



![image.95ION1](/tmp/evince-216861/image.95ION1.png)



- 假 设 本 地 DNS 服 务 器 需 要 解 析 一 个 名 字www.otago.ac.nz。要完成这个任务，它首先与每个 DNS 服务器都知道的一小组根名字服务器中的一个进行通信。（使用命令 dig . NS 或从网页 http://www.root-servers.org/上可以获取这组服务器列表。）给定名字 www.otago.ac.nz，根名字服务器会告诉本 DNS 服务器到其中一台 nzDNS 服务器上查询。然后本地 DNS 服务器会在 nz 服务器上查询名字 www.otago.ac.nz，并收到一个到 ac.nz 服务器上查询的响应。之后本地 DNS 服务器会在 ac.nz 服务器上查询名字www.otago.ac.nz 并被告知查询 otago.ac.nz 服务器。最后本地 DNS 服务器会在 otago.ac.nz 服务器上查询 www.otago.ac.nz 并获取所需的 IP 地址。
- 如果向 gethostbyname()传递了一个不完整的域名，那么解析器在解析之前会尝试补全。域名补全的规则是在/etc/resolv.conf 中定义的（参见 resolv.conf(5)手册）。在默认情况下，解析器至少会使用本机的域名来补全。例如，如果登录机器 oghma.otago.ac.nz 并输入了命令 ssh octavo，得到的 DNS 查询将会以 octavo.otago.ac.nz 作为其名字。

### /etc/services 文件

- 众所周知的端口号是由 IANA 集中注册的，其中每个端口都有一个对应的服务名。由于服务号是集中管理并且不会像 IP 地址那样频繁变化，因此没有必getaddrinfo()要采用 DNS 服务器来管理它们。相反，端口号和服务名会记录在文件/etc/services 中。和 getnameinfo()函数会使用这个文件中的信息在服务名和端口号之间进行转换。



![image.EOVPN1](/tmp/evince-216861/image.EOVPN1.png)



- 协议通常是 tcp 或 udp。可选的（以空格分隔）别名指定了服务的其他名字。此外，每一行中都可能会包含以#字符打头的注释。

- 正如之前指出的那样，一个给定的端口号引用 UDP 和 TCP 的的唯一实体，但 IANA 的策略是将两个端口都分配给服务，即使服务只使用了其中一种协议。如 telnet、ssh、HTTP 以及SMTP，它们都只使用 TCP，但对应的 UDP 端口也被分配给了这些服务。相应地，NTP 只使用 UDP，但 TCP 端口 123 也被分配给了这个服务。在一些情况中，一个服务既会使用 TCP也会使用 UDP，DNS 和 encho 就是这样的服务。最后，还有一些极少出现的情况会将数值相同的 UDP 和 TCP 端口分配给不同的服务，如 rsh 使用 TCP 端口 514，而 syslog daemon（37.5 节）则是使用了 UDP 端口 514。这是因为这些端口在采用现行的 IANA 策略之前就分配出去了。

- /etc/services 文件仅仅记录着名字到数字的映射关系。它不是一种预留机制：在/etc/services中存在一个端口号并不能保证在实际环境中特定的服务就能够绑定到该端口上

### 独立于协议的主机和服务转换

- getaddrinfo()函数将主机和服务名转换成 IP 地址和端口号，它作为过时的 gethostbyname()和 getservbyname()函数的（可重入的）接替者被定义在了 POSIX.1g 中。（使用 getaddrinfo()替换 gethostbyname()能够从程序中删除 IPv4 与 IPv6 的依赖关系。）
- getnameinfo()函数是 getaddrinfo()的逆函数，它将一个 socket 地址结构（IPv4 或 IPv6）转换成包含对应主机和服务名的字符串。这个函数是过时的 gethostbyaddr()和 getservbyport()函数的（可重入的）等价物。

#### getaddrinfo()函数

- 给定一个主机名和服务器名，getaddrinfo()函数返回一个 socket 地址结构列表，每个结构都包含一个地址和端口号。

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int getaddrinfo(const char *host, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **result);

struct addrinfo {
               int              ai_flags;
               int              ai_family;
               int              ai_socktype;
               int              ai_protocol;
               socklen_t        ai_addrlen;
               struct sockaddr *ai_addr;
               char            *ai_canonname;
               struct addrinfo *ai_next;
           };

成功时返回 0，发生错误时返回非零值。
```

- getaddrinfo()以 host、service 以及 hints 参数作为输入，其中 host 参数包含一个主机名或一个以 IPv4 点分十进制标记或 IPv6 十六进制字符串标记的数值地址字符串。（准确地讲， getaddrinfo()接受在 59.13.1 节中描述的更通用的数字和点标记的 IPv4 数值字符串。） service 参数包含一个服务名或一个十进制端口号。hints 参数指向一个 addrinfo 结构，该结构规定了选择通过 result 返回的 socket 地址结构的标准。稍后会介绍有关 hints 参数的更多细节。

- getaddrinfo()会动态地分配一个包含 addrinfo 结构的链表并将 result 指向这个列表的表头。每个 addrinfo 结构包含一个指向与 host 和 service 对应的 socket 地址结构的指针（图 59-3）
- result 参数返回一个结构列表而不是单个结构，因为与在 host、service 以及 hints 中指定的标准对应的主机和服务组合可能有多个。如查询拥有多个网络接口的主机时可能会返回多个地址结构。此外，如果将 hints.ai_socktype 指定为 0，那么就可能会返回两个结构—一个用于 SOCK_DGRAMsocket，另一个用于 SOCK_STREAM socket—前提是给定的 service 同时对 TCP 和 UDP 可用。
- 通过 result 返回的 addrinfo 结构的字段描述了关联 socket 地址结构的属性。 ai_family 字段会被设置成 AF_INET 或 AF_INET6，表示该 socket 地址结构的类型。ai_socktype 字段会被设置成 SOCK_STREAM 或 SOCK_DGRAM，表示这个地址结构是用于 TCP 服务还是用于 UDP服务。ai_protocol 字段会返回与地址族和 socket 类型匹配的协议值。（ai_family、ai_socktype以及 ai_protocol 三个字段为调用 socket()创建该地址上的 socket 时所需的参数提供了取值。）ai_addrlen 字段给出了 ai_addr 指向的 socket 地址结构的大小（字节数）。 in_addr 字段指向 socket地址结构（IPv4 时是一个 in_addr 结构，IPv6 时是一个 in6_addr 结构）。ai_flags 字段未用（它用于 hints 参数）。 ai_canonname 字段仅由第一个 addrinfo 结构使用并且其前提是像下面所描述的那样在 hints.ai_flags 中使用了 AI_CANONNAME 字段。
- 与 gethostbyname()一样，getaddrinfo()可能需要向一台 DNS 服务器发送一个请求，并且这个请求可能需要花费一段时间来完成。同样的过程也适用于 getnameinfo()，具体可参考 59.10.4 节中的描述。



![image.EF6XN1](/tmp/evince-216861/image.EF6XN1.png)

#### hints 参数

- hints 参数为如何选择 getaddrinfo()返回的 socket 地址结构指定了更多的标准。当用作 hints参数时只能设置 addrinfo 结构的 ai_flags、ai_family、ai_socktype 以及 ai_protocol 字段，其他字段未用到，并将应该根据具体情况将其初始化为 0 或 NULL。
- hints.ai_family 字段选择了返回的 socket 地址结构的域，其取值可以是 AF_INET 或AF_INET6（或其他一些 AF_*常量，只要实现支持它们）。如果需要获取所有种类 socket 地址结构，那么可以将这个字段的值指定为 AF_UNSPEC。
- hints.ai_socktype 字段指定了使用返回的 socket 地址结构的 socket 类型。如果将这个字段指定为 SOCK_DGRAM，那么查询将会在 UDP 服务上执行，对应的 socket 地址结构会通过result 返回。如果指定了 SOCK_STREAM，那么将会执行一个 TCP 服务查询。如果将hints.ai_socktype 指定为 0，那么任意类型的 socket 都是可接受的。
- hints.ai_protocol 字段为返回的地址结构选择了 socket 协议。在本书中，这个字段的值总是会被设置为 0，表示调用者接受任何协议。
- hints.ai_flags 字段是一个位掩码，它会改变 getaddrinfo()的行为。这个字段的取值是下列值中的零个或多个取 OR 得来的。

AI_ADDRCONFIG

- 在本地系统上至少配置了一个 IPv4 地址时返回 IPv4 地址（不是 IPv4 回环地址），在本地系统上至少配置了一个 IPv6 系统时返回 IPv6 地址（不是 IPv6 回环地址）。

AI_ALL

- 参见下面对 AI_V4MAPPED 的描述。

AI_CANONNAME

- 如果 host 不为 NULL，那么返回一个指向以 null 结尾的字符串，该字符串包含了主机的规范名。这个指针会在通过 result 返回的第一个 addrinfo 结构中的 ai_canonname 字段指向的缓冲器中返回。

AI_NUMERICHOST

- 强制将 host 解释成一个数值地址字符串。这个常量用于在不必要解析名字时防止进行名字解析，因为名字解析可能会花费较长的时间。

AI_NUMERICSERV

- 将 service 解释成一个数值端口号。这个标记用于防止调用任意的名字解析服务，因为当service 为一个数值字符串时这种调用是没有必要的。

AI_PASSIVE

- 返回一个适合进行被动式打开（即一个监听 socket）的 socket 地址结构。在这种情况下，host 应该是 NULL，通过 result 返回的 socket 地址结构的 IP 地址部分将会包含一个通配 IP 地址（即 INADDR_ANY 或 IN6ADDR_ANY_INIT）。如果没有设置这个标记，那么通过 result 返回的地址结构将能用于 connect()和 sendto()；如果 host 为 NULL，那么返回的 socket 地址结构中的IP 地址将会被设置成回环 IP 地址 （根据所处的域，其值为 INADDR_LOOPBACK 或IN6ADDR_LOOPBACK_INIT）。

AI_V4MAPPED

- 如果在 hints 的 ai_family 字段中指定了 AF_INET6，那么在没有找到匹配的 IPv6 地址时应该在 result 返回 IPv4 映射的 IPv6 地址结构。如果同时指定了 AI_ALL 和 AI_V4MAPPED，那么在 result 中会同时返回 IPv6 和 IPv4 地址，其中 IPv4 地址会被返回成 IPv4 映射的 IPv6 地址结构。
- 正如前面介绍 AI_PASSIVE 时指出的那样，host 可以被指定为 NULL。此外，还可以将service 指定为 NULL，在这种情况下，返回的地址结构中的端口号会被设置为 0（即只关心将主机名解析成地址）。然而无法将 host 和 service 同时指定为 NULL。
- 如果无需在 hints 中指定上述的选取标准，那么可以将 hints 指定为 NULL，在这种情况下会 将 ai_socktype 和 ai_protocol 假 设 为 0 ， 将 ai_flags 假 设 为 （ AI_V4MAPPED |AI_ADDRCONFIG），将 ai_family 假设为 AF_UNSPEC。（glibc 实现有意与 SUSv3 背道而驰，它声称如果 hints 为 NULL，那么会将 ai_flags 假设为 0。）

#### 释放 addrinfo 列表：freeaddrinfo()

- getaddrinfo()函数会动态地为 result 引用的所有结构分配内存（图 59-3），其结果是调用者必须要在不再需要这些结构时释放它们。使用 freeaddrinfo()函数可以方便地在一个步骤中执行这个释放任务。

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int getaddrinfo(const char *host, const char *service,
                const struct addrinfo *hints,
                struct addrinfo **result);

void freeaddrinfo(struct addrinfo *result);

const char *gai_strerror(int errcode);
```

- 如果希望保留 addrinfo 结构或其关联的 socket 地址结构的一个副本，那么必须要在调用freeaddrinfo()之前复制这些结构。

#### 错误诊断：gai_strerror()

- getaddrinfo()在发生错误时会返回表 59-1 中给出的一个非零错误码
- 表 59-1                getaddrinfo()和 getnameinfo()返回的错误码

错 误 常 量                                  描述
EAI_ADDRFAMILY 

在 hints.ai_family 中不存在 host 的地址（没有在 SUSv3 中规定，但大多数实现都对其进行了定义，仅供 getaddrinfo()使用）
EAI_AGAIN 

名字解析过程中发生临时错误（稍后重试）
EAI_BADFLAGS 

在 hints.ai_flags 中指定了一个无效的标记
EAI_FAIL 

访问名字服务器时发生了无法恢复的故障
EAI_FAMILY 

不支持在 hints.ai_family 中指定的地址族
EAI_MEMORY 

内存分配故障
EAI_NODATA 

没有与 host 关联的地址（没有在 SUSv3 中规定，但大多数实现都对其进行了定义，仅供 getaddrinfo()使用）
EAI_NONAME 

未 知 的 host 或 service ， 或 host 和 service 都 为 NULL ， 或 指 定 了AI_NUMERICSERV同时 service 没有指向一个数值字符串
EAI_OVERFLOW 

参数缓冲器溢出
EAI_SERVICE 

hints.ai_socktype 不支持指定的 service（仅供 getaddrinfo()使用）
EAI_SOCKTYPE 

不支持指定的 hints.ai_socktype（仅供 getaddrinfo()使用）
EAI_SYSTEM 

通过 errno 返回的系统错误

- 给定表 59-1 中列出的一个错误码，gai_strerror()函数会返回一个描述该错误的字符串。（该字符串通常比表 59-1 中给出的描述更加简洁。）

```c
#include <sys/socket.h>
#include <netdb.h>

int getnameinfo(const struct sockaddr *addr, socklen_t addrlen,
                char *host, socklen_t hostlen,
                char *serv, socklen_t servlen, int flags);
```

- addr 参数是一个指向待转换的 socket 地址结构的指针，该结构的长度是由 addrlen 指定的。通常，addr 和 addrlen 的值是从 accept()、recvfrom()、getsockname()或 getpeername()调用中获得的。
- 得到的主机和服务名是以 null 结尾的字符串，它们会被存储在 host 和 service 指向的缓冲器中。调用者必须要为这些缓冲器分配空间并将它们的大小传入 hostlen 和 servlen。<netdb.h>头文件定义了两个常量来辅助计算这些缓冲器的大小。 NI_MAXHOST 指出了返回的主机名字符串的最大字节数，其取值为 1025。NI_MAXSERV 指出了返回的服务名字符串的最大字节数，其取值为 32。这两个常量没有在 SUSv3 中得到规定，但所有提供 getnameinfo()的 UNIX 实现都对它们进行了定义。（从 glibc 2.8 起，必须要定义_BSD_SOURCE、_SVID_SOURCE 或_GNU_SOURCE 中的其中一个特性文本宏才能获取 NI_MAXHOST 和NI_MAXSERV 的定义。）
- 如果不想获取主机名，那么可以将 host 指定为 NULL 并且将 hostlen 指定为 0。同样地，
  如果不需要服务名，那么可以将 service 指定为 NULL 并且将 servlen 指定为 0。但是 host 和
  service 中至少有一个必须为非 NULL 值（并且对应的长度参数必须为非零）。
- 最后一个参数 flags 是一个位掩码，它控制着 getnameinfo()的行为，其取值为下面这些常
  量取 OR。

- NI_DGRAM

在默认情况下，getnameinfo()返回与流 socket（即 TCP）服务对应的名字。通常，这是无关紧要的，因为正如 59.9 节中指出的那样，与 TCP 和 UDP 端口对应的服务名通常是相同的，但在一些名字不同的场景中NI_DGRAM 标记会强制返回数据报 socket（即 UDP）服务的名字。

- NI_NAMEREQD

在默认情况下，如果无法解析主机名，那么在 host 中会返回一个数值地址字符串。如果
指定了 NI_NAMEREQD，那么就会返回一个错误（EAI_NONAME）。

- NI_NOFQDN

在默认情况下会返回主机的完全限定域名。指定 NI_NOFQDN 标记会导致当主机位于局
域网中时只返回名字的第一部分（即主机名）。

- NI_NUMERICHOST

强制在 host 中返回一个数值地址字符串。这个标记在需要避免可能耗时较长的 DNS 服务
器调用时是比较有用的。

- NI_NUMERICSERV

强制在 service 中返回一个十进制端口号字符串。这个标记在知道端口号不对应于服务名
时—如它是一个由内核分配给 socket 的临时端口号—以及需要避免不必要的搜索
/etc/services 的低效性时是比较有用的。

### 过时的主机和服务转换 API

```c
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
int inet_aton(const char *cp, struct in_addr *inp);
inet_aton()（“ASCII 到网络”）函数将 str 指向的点分十进制字符串转换成一个网络字节序的 IPv4 地址，转换得到的地址将会返回在 addr 指向的 in_addr 结构中。
inet_aton()函数在转换成功时返回 1，在 str 无效时返回 0。
传入 inet_aton()的字符串的数值部分无需是十进制的，它可以是八进制的（通过前导 0指定），也可以是十六进制的（通过前导 0x 或 0X 指定）。
char *inet_ntoa(struct in_addr in);
给定一个 in_addr 结构（一个 32 位的网络字节序 IPv4 地址）， inet_ntoa()返回一个指向（静态分配的）包含用点分十进制标记法标记的地址的字符串的指针。
由于 inet_ntoa()返回的字符串是静态分配的，因此它们会被后续的调用所覆盖。
```

#### gethostbyname()和 gethostbyaddr()函数

- gethostbyname()和 gethostbyaddr()函数允许在主机名和 IP 地址之间进行转换。现在这些函数已经被 getaddrinfo()和 getnameinfo()所取代了。

```c
#include <netdb.h>
extern int h_errno;

struct hostent *gethostbyname(const char *name);

#include <sys/socket.h>       /* for AF_INET */
struct hostent *gethostbyaddr(const void *addr,
                              socklen_t len, int type);

struct hostent {
               char  *h_name;            /* official name of host */
               char **h_aliases;         /* alias list */
               int    h_addrtype;        /* host address type */
               int    h_length;          /* length of address */
               char **h_addr_list;       /* list of addresses */
           }
h_name 字段返回主机的官方名字，它是一个以 null 结尾的字符串。h_aliases 字段指向
一个指针数组，数组中的指针指向以 null 结尾的包含了该主机名的别名（可选名）的字符串。
    
h_addr_list 字段是一个指针数组，数组中的指针指向这个主机的 IP 地址结构。（一个多宿
主机拥有的地址数超过一个。）这个列表由 in_addr 或 in6_addr 结构构成，通过 h_addrtype 字
段可以确定这些结构的类型，其取值为 AF_INET 或 AF_INET6；通过 h_length 字段可以确定
这些结构的长度。提供 h_addr 定义是为了与在 hostent 结构中只返回一个地址的早期实现（如
4.2BSD）保持向后兼容，一些既有代码依赖于这个名字（因此无法感知多宿主机）。

在现代版本的 gethostbyname()中也可以将 name 指定为一个数值 IP 地址字符串，即 IPv4 的数
字和点标记法与 IPv6 的十六进制字符串标记法。在这种情况下不会执行任何的查询工作；相反，
name 会被复制到 hostent 结构的 h_name 字段，h_addr_list 会被设置成 name 的二进制表示形式。

gethostbyaddr()函数执行 gethostbyname()的逆操作。给定一个二进制 IP 地址，它会返回一
个包含与配置了该地址的主机相关的信息的 hostent 结构。
    
在发生错误时（如无法解析一个名字），gethostbyname()和 gethostbyaddr()都会返回一个 NULL
指针并设置全局变量 h_errno。正如其名字所表达的那样，这个变量与 errno 类似（gethostbyname(3)
手册描述了这个变量的可取值），herror()和 hstrerror()函数类似于 perror()和 strerror()。
    
herror()函数（在标准错误上）显示了在 str 中给出的字符串，后面跟着一个冒号(:)，然后
再显示一条与当前位于 h_errno 中的错误对应的消息。或者可以使用 hstrerror()获取一个指向
与在 err 中指定的错误值对应的字符串的指针。
```

```c
#include <netdb.h>
extern int h_errno;

void herror(const char *s);

const char *hstrerror(int err);

herror()函数（在标准错误上）显示了在 str 中给出的字符串，后面跟着一个冒号(:)，然后
再显示一条与当前位于 h_errno 中的错误对应的消息。或者可以使用 hstrerror()获取一个指向
与在 err 中指定的错误值对应的字符串的指针。
```

#### getserverbyname()和 getserverbyport()函数

- getservbyname()和 getservbyport()函数从/etc/services 文件（59.9 节）中获取记录。现在这些函数已经被 getaddrinfo()和 getnameinfo()所取代了。

```C
#include <netdb.h>

struct servent *getservbyname(const char *name, const char *proto);

struct servent *getservbyport(int port, const char *proto);

struct servent {
               char  *s_name;       /* official service name */
               char **s_aliases;    /* alias list */
               int    s_port;       /* port number */
               char  *s_proto;      /* protocol to use */
           }
getservbyname()函数查询服务名（或其中一个别名）与 name 匹配以及协议与 proto 匹配的记录。proto 参数是一个诸如 tcp 或 udp 之类的字符串，或者也可以将它设置为 NULL。如果将 proto 指定为 NULL，那么就会返回任意一个服务名与 name 匹配的记录。（这种做法通常已经足够了，因为当拥有同样名字的 UDP 和 TCP 记录都位于/etc/services 文件时，它们通常使用同样的端口号。）如果找到了一个匹配的记录，那么 getservbyname()会返回一个指向静态分配的如下类型的结构的指针。
    
一般来讲，调用 getservbyname()只为了获取端口号，该值会通过 s_port 字段返回。
    
getservbyport()函数执行 getservbyname()的逆操作，它返回一个 servent 记录，该记录包含了/etc/services 文件中端口号与 port 匹配、协议与 proto 匹配的记录相关的信息。同样，可以将 proto指定为 NULL，这样这个调用就会返回任意一个端口号与 port 中指定的值匹配的记录。（在前面提到的一些同一个端口号被映射到不同的 UDP 和 TCP 服务名的情况下可能不会返回期望的结果。）
```

#### UNIX 与 Internet domain socket 比较

- 当编写通过网络进行通信的应用程序时必须要使用 Internet domain socket，但当位于同一系统上的应用程序使用 socket 进行通信时则可以选择使用 Internet 或 UNIX domain socket。在这种情况下该使用哪个 domain？为何使用这个 domain 呢？
- 编写只使用 Internet domain socket 的应用程序通常是最简单的做法，因为这种应用程序既能运行于同一个主机上，也能运行在网络中的不同主机上。但之所以要选择使用 UNIX domainsocket 是存在几个原因的。

1. 在一些实现上，UNIX domain socket 的速度比 Internet domain socket 的速度快。
2. 可以使用目录（在 Linux 上是文件）权限来控制对 UNIX domain socket 的访问，这样只有运行于指定的用户或组 ID 下的应用程序才能够连接到一个监听流 socket 或向一个数据报 socket 发送一个数据报，同时为如何验证客户端提供了一个简单的方法。使用Internet domain socket 时如果需要验证客户端的话就需要做更多的工作了。
3. 使用 UNIX domain socket 可以像 61.13.3 节中总结的那样传递打开的文件描述符和发送者的验证信息。



#### 总结

- Internet domain socket 允许位于不同主机上的应用程序通过一个 TCP/IP 网络进行通信。一个 Internet domain socket 地址由一个 IP 地址和一个端口号构成。在 IPv4 中，一个 IP 地址是一个 32 位的数字，在 IPv6 中则是一个 128 位的数字。Internet domain 数据报 socket 运行于UDP 上，它提供了无连接的、不可靠的、面向消息的通信。Internet domain 流 socket 运行于TCP 上，它为相互连接的应用程序提供了可靠的、双向字节流通信信道。
- 不同的计算机架构使用不同的方式来表示数据类型。如整数可以以小端形式存储也可以以大端形式存储，并且不同的计算机可能使用不同的字节数来表示诸如 int 和 long 之类的数值类型。这些差别意味着当在通过网络连接的异构机器之间传输数据时需要采用某种独立于架构的表示。本章指出了存在多种信号编集标准来解决这个问题，同时还描述了被很多应用程序所采用的一个简单的解决方案：将所有传输的数据编码成文本形式，字段之间使用预先指定的字符（通常是换行符）分隔。
- 本章介绍了一组用于在 IP 地址的（数值）字符串表示（IPv4 是点分十进制，IPv6 是十六进制字符串）和其二进制值之间进行转换的函数，然而一般来讲最好使用主机和服务名而不是数字，因为名字更容易记忆并且即使在对应的数字发生变化时也能继续使用。此外，还介绍了用于将主机和服务名转换成数值表示及其逆过程的各种函数。将主机和服务名转换成socket 地址的现代函数是 getaddrinfo()，但读者在既有代码中会经常看到早期的 gethostbyname()和 getservbyname()函数。
- 对主机名转换的思考引出了对 DNS 的讨论，它实现了一个分布式数据库提供层级目录服务。DNS 的优点是数据库的管理不再是集中的了。相反，本地区域管理员可以更新他们所负责的数据库层级部分，并且 DNS 服务器可以与另一台服务器进行通信以便解析一个主机名。

### chap61 SOCKET：高级主题

```c
#include <sys/socket.h>

int shutdown(int sockfd, int how);

SHUT_RD
关闭连接的读端。之后的读操作将返回文件结尾（0）。数据仍然可以写入到套接字上。
在 UNIX 域流式套接字上执行了 SHUT_RD 操作后，对端应用程序将接收到一个 SIGPIPE 信
号，如果继续尝试在对端套接字上做写操作的话将产生 EPIPE 错误。如 61.6.6 节中讨论的，
SHUT_RD 对于 TCP 套接字来说没有什么意义。

SHUT_WR
关闭连接的写端。一旦对端的应用程序已经将所有剩余的数据读取完毕，它就会检测到
文件结尾。后续对本地套接字的写操作将产生 SIGPIPE 信号以及 EPIPE 错误。而由对端写入
的数据仍然可以在套接字上读取。换句话说，这个操作允许我们在仍然能读取对端发回给我
们的数据时，通过文件结尾来通知对端应用程序本地的写端已经关闭了。 SHUT_WR 操作在 ssh
和 rsh 中都有用到（参见[Stevens，1994]中的 18.5 节）
。在 shutdown()中最常用到的操作就是
SHUT_WR，有时候也被称为半关闭套接字。
    
SHUT_RDWR
将连接的读端和写端都关闭。这等同于先执行 SHUT_RD，跟着再执行一次 SHUT_WR操作。
```

```c
#include <sys/types.h>
#include <sys/socket.h>

ssize_t recv(int sockfd, void *buf, size_t len, int flags);

int send(int s, const void *msg, size_t len, int flags);

MSG_DONTWAIT
让 recv()以非阻塞方式执行。如果没有数据可用，那么 recv()不会阻塞而是立刻返回，
伴随的错误码为 EAGAIN。我们可以通过 fcntl()把套接字设为非阻塞模式（O_NONBLOCK）
从而达到相同的效果。区别在于 MSG_DONTWAIT 允许我们在每次调用中控制非阻塞行为。
MSG_OOB
在套接字上接收带外数据。我们将在 61.13.1 节中简要描述这个特性。
    
MSG_PEEK
从套接字缓冲区中获取一份请求字节的副本，但不会将请求的字节从缓冲区中实际移除。
这份数据稍后可以由其他的 recv()或 read()调用重新读取。
    
MSG_WAITALL
通常，recv()调用返回的字节数比请求的字节数（由 length 参数指定）要少，而那些字节实
际上还在套接字中。指定了 MSG_WAITALL 标记后将导致系统调用阻塞，直到成功接收到 length
个字节。但是，就算指定了这个标记，当出现如下情况时，该调用返回的字节数可能还是会少
于请求的字节。这些情况是：（a）捕获到一个信号；（b）流式套接字的对端终止了连接；（c）遇到了带外数据字节（参见 61.13.1 节）；（d）从数据报套接字接收到的消息长度小于 length 个
字节；（e）套接字上出现了错误。（MSG_WAITALL 标记可以取代我们在程序清单 61-1 中给出
的 readn()函数，区别在于我们实现的 readn()函数在被信号处理例程中断后会重新得到调用。）
除了 MSG_DONTWAIT 之外，以上所有标记都在 SUSv3 中有规范。MSG_DONTWAIT
也存在于其他一些 UNIX 实现中。这个标记加入到套接字 API 的时间比较晚，在一些老式的实现中并不存在。
对于 send()，flags 参数可以是以下值相或的结果。
    
MSG_DONTWAIT
让 send()以非阻塞方式执行。如果数据不能立刻传输（因为套接字发送缓冲区已满），那
么该调用不会阻塞，而是调用失败，伴随的错误码为 EAGAIN。和 recv()一样，可以通过对套
接字设定 O_NONBLOCK 标记来实现同样的效果。
    
MSG_MORE（从 Linux 2.4.4 开始）
在 TCP 套接字上，这个标记实现的效果同套接字选项 TCP_CORK（见 61.4 节）完成的
功能相同。区别在于该标记可以在每次调用中对数据进行栓塞处理。从 Linux 2.6 版以来，这个
标记也可以用于数据报套接字，但所代表的意义有所不同。在连续的 send()或 sendto()调用中
传输的数据，如果指定了 MSG_MORE 标记，那么数据会打包成一个单独的数据报。仅当下一
次调用中没有指定该标记时数据才会传输出去。（Linux 也提供了类似的 UDP_CORK 套接字
选项，这将导致在连续的 send()或 sendto()调用中传输的数据会累积成一个单独的数据报，当
取消 UDP_CORK 选项时才会将其发送出去。）MSG_MORE 标记对 UNIX 域套接字没有任何效果。
    
MSG_NOSIGNAL
当在已连接的流式套接字上发送数据时，如果连接的另一端已经关闭了，指定该标记后将
不会产生 SIGPIPE 信号。相反，send()调用会失败，伴随的错误码为 EPIPE。这和忽略 SIGPIPE
信号所得到的行为相同。区别在于该标记可以在每次调用中控制信号发送的行为。
   
MSG_OOB
在流式套接字上发送带外数据。参见 61.13.1 节。
以上标记中只有 MSG_OOB 在 SUSv3 中有规范。MSG_DONTWAIT 标记也在其他一些
UNIX 实现中出现过，而 MSG_NOSIGNAL 和 MSG_MORE 都是 Linux 专有的。
```

```c
为了传输文件，我们必须使用两个系统调用（可能需要在循环中多次调用）：一个用来将文件内容从内核缓冲区 cache 中拷贝到用户空间，另一个用来将用户空间缓冲区拷贝回内核空间，以此才能通过套接字进行传输。图 61-1
的左侧展示了这种场景。如果应用程序在发起传输之前根本不对文件内容做任何处理的话，
那么这种两步式的处理就是一种浪费。系统调用 sendfile()被设计为用来消除这种低效性。如
图 61-1 右侧所示，当应用程序调用 sendfile()时，文件内容会直接传送到套接字上，而不会经过
用户空间。这种技术被称为零拷贝传输（zero-copy transfer）。

#include <sys/sendfile.h>

ssize_t sendfile(int out_fd, int in_fd, off_t *offset, size_t count);

系统调用 sendfile()在代表输入文件的描述符 in_fd 和代表输出文件的描述符 out_fd 之
间传送文件内容（字节）。描述符 out_fd 必须指向一个套接字。参数 in_fd 指向的文件必须
是可以进行 mmap()操作的。在实践中，这通常表示一个普通文件。这些局限多少限制了
sendfile()的使用。我们可以使用 sendfile()将数据从文件传递到套接字上，但反过来就不行。
另外，我们也不能通过 sendfile()在两个套接字之间直接传送数据。
    
如果参数 offset 不是 NULL，它应该指向一个 off_t 值，该值指定了起始文件的偏移量，
意即从 in_fd 指向的文件的这个位置开始，可以传输字节。这是一个传入传出参数（又叫值一
结果参数）。在返回的值中，它包含从 in_fd 传输过来的紧靠着最后一个字节的下一个字节的
偏移量 。在这里，serdfile()不会更改 in_fd 的文件偏移量。
    
如果参数 offset 指定为 NULL 的话，那么从 in_fd 传输的字节就从当前的文件偏移量处开
始，且在传输时会更新文件偏移量以反映出已传输的字节数。

参数 count 指定了请求传输的字节数。如果在 count 个字节完成传输前就遇到了文件结尾符，那
么只有文件结尾符之前的那些字节能传输。调用成功后，sendfile()会返回实际传输的字节数。
```

##### TCP_CORK 套接字选项

- 要进一步提高 TCP 应用使用 sendfile()时的性能，采用 Linux 专有的套接字选项 TCP_CORK
  常常会很有帮助。例如，Web 服务器传送页面给浏览器，作为对请求的响应。Web 服务器的响
  应由两部分组成：HTTP 首部，也许会通过 write()来输出；页面数据，可以通过 sendfile()来输
  出。在这种场景下，通常会传输 2 个 TCP 报文段：HTTP 首部在第一个（非常小）报文段中，
  而页面数据在第二个报文段中发送。这对网络带宽的利用率是不够高效的。可能还会在发送和
  接收 TCP 报文时做些不必要的工作，因为在许多情况下 HTTP 首部和页面数据都比较小，足以
  容纳在一个单独的 TCP 报文段中。套接字选项 TCP_CORK 正是被设计为用来解决这种低效性。
- 当在 TCP 套接字上启用了 TCP_CORK 选项后，之后所有的输出都会缓冲到一个单独的 TCP
  报文段中，直到满足以下条件为止：已达到报文段的大小上限、取消了 TCP_CORK 选项、套接字
  被关闭，或者当启用 TCP_CORK 后，从写入第一个字节开始已经经历了 200 毫秒。
  （如果应用程序忘记取消 TCP_CORK 选项，那么超时时间可确保被缓冲的数据能得以传输。）
- 我们通过 setsockopt()系统调用（见 61.9 节）来启用或取消 TCP_CORK 选项。下面的代
  码（省略错误检查）说明了在我们假想的 HTTP 服务器例子中应该如何使用 TCP_CORK 选项。

```c
int optval;
optval = 1;
setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, sizeof(optval));
wrte(sockfd,...);
sendfile(sockfd,...);
...
optval = 0;
setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, sizeof(optval));
```

- 在我们的应用中，通过构建一个单独的数据缓冲区，可以避免出现需要发送两个报文段
  的情况，之后可以通过一个单独的 write()将缓冲区数据发送出去。（可选的方式是，我们可以通
  过 writev()将两个独立的缓冲区结合为一次单独的输出操作。）但是，如果我们希望将 sendfile()
  的零拷贝高效性和传输文件数据时在第一个报文段中包含 HTTP 首部信息的能力结合起来的
  话，那么我们需要用到 TCP_CORK。

### 获取套接字地址

- getsockname()和 getpeername()这两个系统调用分别返回本地套接字地址以及对端套接字地址。

```c
#include <sys/socket.h>

int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

sockfd 表示指向套接字的文件描述符，而 addr 是一个指向 sockaddr 结
构体的指针，该结构体包含着套接字的地址。这个结构体的大小和类型取决于套接字域。
Addrlen 是一个保存结果值的参数。在执行调用之前，addrlen 应该被初始化为 addr 所指向的缓
冲区空间的大小。调用返回后，addrlen 中包含实际写入到这个缓冲区中的字节数。

getsockname()可以返回套接字地址族，以及套接字所绑定到的地址。如果套接字绑定到了另一个程序（比如 inetd(8)），且套接字文件描述符在经过 exec()调用后仍然得到保留，那么此时 getsockname()就能派上用场了。

当隐式绑定到一个 Internet 域套接字上时，如果我们想获取内核分配给套接字的临时端口号，那么调用 getsockname()也是有用的。内核会在出现如下情况时执行一个隐式绑定。
    已经在 TCP 套接字上执行了 connect()或 listen()调用，但之前还没有通过 bind()绑定到一个地址上。
    当在 UDP 套接字上首次调用 sendto()时，该套接字之前还没有绑定到地址上。
    调用 bind()时将端口号（sin_port）指定为 0。这种情况下 bind()会为套接字指定一个IP 地址，但内核会选择一个临时的端口号。

系统调用 getpeername()返回流式套接字连接中对端套接字的地址。如果服务器想找出发
出连接的客户端地址，这个调用就特别有用，主要用于 TCP 套接字上。对端套接字的地址信
息也可以在执行 accept()时获取，但是如果服务器进程是由另一个程序调用的，而 accept()是
由该程序（比如 inetd）所执行，那么服务器进程可以继承套接字文件描述符，但由 accept()
返回的地址信息就不存在了。
```

