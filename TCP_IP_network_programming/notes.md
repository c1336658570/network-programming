# TCP/IP网络编程

## Part01 开始网络编程

### 理解网络编程和套间字

```c
//server
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
sock = socket(PF_INET, SOCK_STREAM, 0);

#include <sys/types.h>
#include <sys/socket.h>

int bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen);
bzero(&serv_addr, sizeof(serv_addr));
bzero(&clnt_addr, sizeof(clnt_addr));
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
serv_addr.sin_port = atoi(argv[1]);
bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
ADDRESS FORMAT(地址格式)
       一个 IP 套接字地址定义为一个 IP 接口地址和一个端口号的组合． 基本 IP 协
       议不会提供端口号,它们通过更高层次的协议如  udp(7) 和 tcp(7) 来实现． 对
       于raw套接字， sin_port 设置为IP协议．

              struct sockaddr_in {
              sa_family_t sin_family; /* 地址族: AF_INET */
              u_int16_t sin_port; /* 按网络字节次序的端口 */
              struct in_addr sin_addr; /* internet地址 */
              };

              /* Internet地址. */
              struct in_addr {
              u_int32_t s_addr; /* 按网络字节次序的地址 */
              };

#include <sys/socket.h>

int listen(int s, int backlog);
listen(serv_sock, 50);

#include <sys/types.h>
#include <sys/socket.h>

int accept(int s, struct sockaddr *addr, socklen_t *addrlen);
int clnt_addr_size;
clnt_addr_size = sizeof(clnt_addr);
clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);

//clinet
struct sockaddr_in serv_addr;
sock = socket(PF_INET, SOCK_STREAM, 0);
bzero(&serv_addr, sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
serv_addr.sin_port = atoi(argv[2]);
connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
```

```c
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

int socket(int domain, int type, int protocol);
domain
    PF_INET IPv4互联网协议族
    PF_INET6 IPv6互联网协议族
    PF_LOCAL 本地通信的UNIX协议族
    PF_PACKET 底层套间字的协议族
    PF_IPX IPX Novell协议族
type
    面向连接的套间字（SOCK_STREAM）
    	传输过程中数据不会消失
    	按序传输数据
    	传输的数据不存在数据边界
    		可靠的、按序传递的、基于字节的面向连接的数据传输方式的套间字
    
    面向消息的套间字（SOCK_DGRAM）
    	强调快速传输而非传输顺序
    	传输的数据可能丢失也可能损毁
    	传输的数据有数据边界
    	限制每次传输的数据大小
    		不可靠的、不按序传递的、以数据的高速传输为目的的套间字
    
面向连接
//满足PF_INET,SOCK_STREAM的协议只有IPPROTO_TCP
int tcp_socket =socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

面向消息
//满足PF_INET,SOCK_DGRAM的协议只有IPPROTO_TCP
int udp_socket =socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);


```

### 地址族与数据序列

```c
struct sockaddr_in {
              sa_family_t sin_family; /* 地址族: AF_INET */
              u_int16_t sin_port; /* 按网络字节次序的端口 */
              struct in_addr sin_addr; /* internet地址 */
    		  char sin_zero[8]; /* 不使用 */
              };

              /* Internet地址. */
              struct in_addr {
              u_int32_t s_addr; /* 按网络字节次序的地址 */
              };

字节序转换
#include <arpa/inet.h>

uint32_t htonl(uint32_t hostlong);

uint16_t htons(uint16_t hostshort);

uint32_t ntohl(uint32_t netlong);

uint16_t ntohs(uint16_t netshort);

将字符串信息转换为网络字节序的整数型

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
功能与inet_aton相反
该函数返回的字符串在内部申请了空间并保存了字符串，该函数调用后，应立即将字符串信息复制到其他内存空间。防止再次调用该函数覆盖以前保存的信息

struct in_addr inet_makeaddr(in_addr_t net, in_addr_t host);

in_addr_t inet_lnaof(struct in_addr in);

in_addr_t inet_netof(struct in_addr in);

网络地址初始化
struct sockaddr_in addr;
char *serv_ip = "211.17.168.13"; //声明IP地址字符串
char *serv_port = "9190"; //声明端口号字符串
memset(&addr, 0, sizeof(addr)); //结构体变量addr所有成员初始化为0
addr.sin_family = AF_INET; //指定地址族
addr.sin_addr = inet_addr(serv_ip); //基于字符串的IP地址初始化
addr.sin_port = htons(atoi(serv_port)); //基于字符串的端口号初始化

服务器地址结构初始化
INADDR_ANY
struct sockaddr_in addr;
char *serv_port = "9190";
memset(&addr, 0, sizeof(addr));
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = htonl(INADDR_ANY);
addr.sin_port = htons(atoi(serv_port)); 

向套间字分配网络地址
int bind(int sockfd, struct sockaddr *my_addr, socklen_t addrlen);
							成功返回0，失败返回-1
sockfd 要分配地址信息（IP地址和端口号）的套间字文件描述符
my_addr 存有地址信息的结构体变量的地址值
addrlen 第二个结构体变量的长度
int serv_sock, clnt_sock;
struct sockaddr_in serv_addr, clnt_addr;
char *serv_port = "9190";
/* 创建服务器端套间字（监听套间字） */
serv_sock = socket(PF_INET, SOCK_STREAM, 0);

/* 地址信息初始化 */
memset(&serv_addr, 0, sizeof(addr));
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
serv_addr.sin_port = htons(atoi(serv_port)); 

/* 分配地址信息 */
bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
```

### 基于TCP的服务器端/客户端（1）

```c
TCP服务器端的默认函数调用顺序
socket();
bind();
listen();
accept();
read()/write();
close();

#include <sys/socket.h>

int listen(int s, int backlog)
    成功返回0，失败返回-1
s 希望进入等待连接请求状态的套间字文件描述符，传递的描述符套间字参数成为服务器端套间字（监听套间字）
backlog 连接请求等待队列（Queue）的长度，若为5，则队列长度为5，表示最多使5个连接请求进入队列
    
#include <sys/types.h>
#include <sys/socket.h>

int accept(int s, struct sockaddr *addr, socklen_t *addrlen);
	成功返回创建的套间字文件描述符，失败时返回-1
s 服务器套间字的文件描述符
addr 保存发起连接请求的客户端的地址信息的变量地址值，调用函数后传递来的地址变量参数填充客户端地址信息
addrlen 第二个参数addr结构体的长度，但是存有长度的变量地址。函数调用完成后，改变量即被填入客户端地址长度
        
TCP客户端的默认函数调用顺序
socket();
connect();
read() / write();
close();

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

int connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);
	成功返回0，失败返回-1。
sock 客户端套间字文件描述符
servaddr 保存目标服务器端地址信息的变量地址值
addrlen 以字节为单位传递已传递给第二个结构体参数servaddr的地址变量长度
客户端调用connect函数后，以下情况才会返回
服务器端接收连接请求
发生断网等异常情况而中断连接请求
```



### 基于TCP的服务器端/客户端（2）

```c
TCP原理

TCP套间字中的I/O缓冲
I/O缓冲在每个TCP套间字中单独存在。
I/O缓冲在创建套间字时自动生成。
即使关闭套间字也会继续传递输出缓冲区中的遗留的数据。
关闭套间字将丢失输入缓冲中的数据
```



### 基于UDP的服务器端/客户端

```c
流控制是区分TCP和UDP的最重要的标志
    
UDP的高效使用
TCP比UDP慢的原因通常有以下两点。
收发数据前后进行的连接设置及清除过程
收发数据过程中为保证可靠性而添加的流程控制
    
UDP不需要listen和accept函数，UDP进行通信只需要一个套间字。
UDP不建立连接，所以每次传输信息需要添加目标地址信息。
#include <sys/types.h>
#include <sys/socket.h>

ssize_t send(int sockfd, const void *buf, size_t len, int flags);

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);
	成功返回传输的字节数，失败返回-1
sockfd 用于传输数据的UDP套间字文件描述符
buf 保存待传输数据的缓冲地址值
len 待传输的数据长度，以字节为单位
flags 可选参数，若没有则传递0
dest_addr 存有目标地址信息的sockaddr结构体变量的地址值
addrlen 传递给参数dest_addr的地址结构体变量长度
        
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);

#include <sys/types.h>
#include <sys/socket.h>

ssize_t recv(int sockfd, void *buf, size_t len, int flags);

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);
sockfd 用于接收数据的UDP文件描述符
buf 保存接收输数据的缓冲地址值
len 可接受的最大字节数，以字节为单位
flags 可选参数，若没有则传递0
dest_addr 存有目标地址信息的sockaddr结构体变量的地址值
addrlen 传递给参数dest_addr的地址结构体变量长度

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);

UDP的数据传输特性和调用connect函数

TCP数据传输中不存在边界，即数据传输过程中调用I/O函数的次数不具有任何意义
相反，UDP是具有数据边界协议，传输中调用I/O函数的次数非常重要。因此，输入函数的调用应和输出函数的调用次数完全一致，这样才能保证数据全部被接收
    
已连接UDP套间字和未连接UDP套间字
UDP中无须注册目标IP和端口号，因此sendto函数传输大致分文以下三阶段
第一阶段：向UDP套间字注册目标IP和端口号
第二阶段：传输数据
第三阶段：删除UDP套间字中注册的目标地址信息
每次调用sendto重复上述过程，每次都变更目标地址，因此可以利用统一UDP套间字向不同目标传输数据。这种未注册目标地址信息的套间字称为未连接套间字，反之则为连接connected套间字。
有时需要创建连接的UDP套间字，因为向同一个目标调用sendto三次，会导致1，3阶段重复3次，每次重复占总时间的1/3。
UDP套间字调用connect并不意味着与对方UDP套间字连接，而是向UDP套间字注册目标IP和端口信息。这样每次调用sendto只需传输数据。不止能调用sendto、recvfrom，而且还可以调用write,read等函数
```

### 优雅地端开套间字连接

```c
#include <sys/socket.h>

int shutdown(int sockfd, int how);
	成功时返回0，失败时返回-1。

sock 需要断开的套间字文件描述符。
howto 传递断开方式信息。
SHUT_RD：端开输入流
SHUT_WR：端开输出流。
SHUT_RDWR：同时端开I/O流。
调用SHUT_WR断开输出流，如果输出流缓冲区中还有未传输的数据，则将传递至目标主机。
```

### 域名系统

```c
ping www.naver.com			查看目标DNS的IP地址
nslookup  会提示进一步输入信息，此时输入server得到默认DNS服务器地址
    
利用域名获取IP地址
#include <netdb.h>
extern int h_errno;

struct hostent *gethostbyname(const char *name);
	成功时返回hostent结构体地址，失败时返回NULL指针
   struct hostent {
               char  *h_name;            /* official name of host */
               char **h_aliases;         /* alias list */
               int    h_addrtype;        /* host address type */
               int    h_length;          /* length of address */
               char **h_addr_list;       /* list of addresses */
           }
h_name
    存有官方域名。官方域名代表某一主页，但实际上，一些著名公司的域名并未使用官方域名注册
h_aliases
    可以通过多个域名访问同一主页。同一IP可以绑定多个域名，因此，除官方域名外还可以指定其他域名。这些信息可以通过h_aliases获得。
h_addrtype
    gethostbyname函数不仅支持IPv4，还支持IPv6。因此可以通过此变量获取保存在h_addr_list的IP地址的地址族信息。若是IPv4，则此变量存有AF_INET。
h_length
    保存IP地址长度。若是IPv4地址，因为是4字节，则保存4；IPv6时，因为时16个字节，故保存16。
h_addr_list
    最重要的成员。 通过此变量以整数形式保存域名对应的IP地址。另外，用户较多的网站可能分配多个IP给同一域名，利用多个服务器进行负载均衡。此时同样可以通过此变量获取IP地址信息。
    
char *inet_ntoa(struct in_addr in);
typedef uint32_t in_addr_t;
struct in_addr {
    in_addr_t s_addr;
};
将整数形式的域名所对应的IP地址转为字符串类型，返回指针
for (int i = 0; host->h_addr_list[i]; ++i)
{
    printf("IP addr %d: %s \n", i + 1, inet_ntoa(*(struct in_addr *)host>h_addr_list[i]));
}

利用IP地址获取域名
include <sys/socket.h>       /* for AF_INET */
struct hostent *gethostbyaddr(const void *addr, socklen_t len, int type);
	成功时返回hostent结构体变量地址值，失败时返回NULL指针
addr 含有IP地址信息的in_addr结构体指针。为了同时传递IPv4地址之外的其他信息，该变量的类型声明为void指针
len 向第一个参数传递的地址信息的字节数，IPv4为4，IPv6为16
type 传递地址族信息，IPv4时为AF_INET，IPv6时为AF_INET6。
```

### 套间字的多种选项

```c
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

//读取套间字选项
int getsockopt(int sockfd, int level, int optname,
               void *optval, socklen_t *optlen);
	成功时返回0，失败时返回-1。
sock 用于查看选项套间字文件描述符
level 要查看的可选项的协议层
optname 要查看的可选项名
optval 保存查看结果的缓冲地址值。
optlen 向第四个参数optval传递缓冲大小。调用函数后，该变量中保存通过第四个参数返回的可选信息的字节数
    
int setsockopt(int sockfd, int level, int optname,
               const void *optval, socklen_t optlen);
	成功时返回0，失败时返回-1。
sock 用于更改可选项的套间字文件描述符
level 要更改的可选项协议层
optname 要更改的可选项名
optval 保存要更改的选项信息的缓冲地址值
optlen 向第四个参数optval传递的可选项信息的字节数
        
套间字的类型在创建时决定，以后不能更改

SO_SNDBUF & SO_RCVBUF
setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *)&rcv_buf, sizeof(rcv_buf)); 设置输入缓冲大小
getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (void *)&rcv_buf, &len); 获取输入缓冲大小
setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *)&snd_buf, sizeof(snd_buf)); 设置输出缓冲大小
getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void *)&snd_buf, &len); 获取输出缓冲大小

SO_REUSEADDR
设置端口复用
optlen=sizeof(option);
option=TRUE;
setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, &option, optlen);

#include <netinet/tcp.h>
禁用Nagle算法
int opt_val = 1;
setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&opt_val, sizeof(opt_val));
通过TCP_NODELAY的值查看Nagle算法的设置状态
int opt_val;
socklen_t opt_len;
opt_len = sizeof(opt_val);
getsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (void *)&opt_val, &opt_len);
如果正在使用Nagle算法，opt_val变量中会保存0，如果已禁用Nagle算法，则保存1
```



### 多进程服务器端

```c
通过调用fork函数创建进程
pid_t fork(void);

销毁僵尸进程
#include <sys/types.h>
#include <sys/wait.h>

pid_t wait(int *wstatus);
pid_t waitpid(pid_t pid, int *wstatus, int options);
int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);
#include <signal.h>
typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
注册信号处理函数

#include <signal.h>
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
struct sigaction {
               void     (*sa_handler)(int);
               void     (*sa_sigaction)(int, siginfo_t *, void *);
               sigset_t   sa_mask;
               int        sa_flags;
               void     (*sa_restorer)(void);
           };
发生信号将唤醒由于调用sleep函数而进入阻塞状态的进程。
```

#### 进程间通信

```c
#include <unistd.h>

       /* On Alpha, IA-64, MIPS, SuperH, and SPARC/SPARC64; see NOTES */
       struct fd_pair {
           long fd[2];
       };
       struct fd_pair pipe();

       /* On all other architectures */
       int pipe(int pipefd[2]);
```

