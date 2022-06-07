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

