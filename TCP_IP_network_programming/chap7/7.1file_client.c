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
    int clnt_sock;
    FILE *fp;
    char buf[BUF_SIZE];
    int read_cnt;
    struct sockaddr_in serv_addr;

    if (argc != 3)
    {
        printf("Usage: %s <IP> <prot>\n", argv[0]);
        exit(-1);
    }

    fp = fopen("reveive.dat", "wb");
    clnt_sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[1], &serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(atoi(argv[2]));

    connect(clnt_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    while ((read_cnt = read(clnt_sock, buf, BUF_SIZE)) != 0)
    {
        fwrite((void *)buf, 1, BUF_SIZE, fp);
    }

    puts("Reveived file data");
    write(clnt_sock, "Thank you", 10);
    fclose(fp);
    close(clnt_sock);

    return 0;
}

void error_handling(char *message)
{

    fputs(message, stderr);
    fputs("\n", stderr);
    exit(1);
}