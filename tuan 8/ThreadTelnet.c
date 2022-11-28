#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <malloc.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_CONN_NUM 1024
#define INVALID_SOCKET -1
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

int g_cfd[MAX_CONN_NUM];
pthread_t g_tid[MAX_CONN_NUM];
int g_count = 0;
int exitflag = 0;
int sfd = 0;

int SendData(int fd, char* data, int len)
{
    int sent = 0;
    int tmp = 0;
    do
    {
        tmp = send(fd, data + sent, len - sent, 0);
        sent += tmp;
    } while (tmp >= 0 && sent < len);
    return sent;
}

int RecvData(int fd, char* data, int maxlen)
{
    int received = 0;
    int blocksize = 1024;
    int tmp = 0;
    do
    {
        tmp = recv(fd, data + received, blocksize, 0);
        received += tmp;
    } while (tmp >= 0 && received < maxlen && tmp == blocksize);
    return received;
}

int main()
{
    int sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Tạo socket
    SOCKADDR_IN saddr, caddr;
    int clen = sizeof(caddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5555);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int error = bind(sfd, (SOCKADDR*)&saddr, sizeof(saddr)); //gán địa chỉ cho socket 
    if (error != -1)
    {
        listen(sfd, 10); //chỉ định socket lắng nghe kết nối

        while (exitflag == 0)
        {
            int cfd = accept(sfd, (SOCKADDR*)&caddr, &clen);
            if (cfd != INVALID_SOCKET)
            {
                g_cfd[g_count] = cfd;
                pthread_create(&g_tid[g_count], NULL, myfunc, &g_cfd[g_count]);
                g_count++;
                printf("Client Count: %d\n", g_count + 1);
                printf("CFD: %d\n", cfd);
            }
        }
        printf("While END\n");

        for (int i = 0;i < g_count;i++)
        {
            pthread_join(g_tid[i], NULL);
        }
        close(sfd);    
    }else
        printf("Failed to bind\n");
    close(sfd);
}