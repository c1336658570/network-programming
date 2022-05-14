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



