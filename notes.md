# TCP/IP网络编程

## 开始网络编程

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

