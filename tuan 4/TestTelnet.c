//
// Created by monleak on 31/10/2022.
//
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

#define INVALID_SOCKET -1
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

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
    int cfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5555);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int error = connect(cfd, (SOCKADDR*)&saddr, sizeof(saddr));
    if (error != -1)
    {
        char buffer[1000] = { 0 };
        RecvData(cfd, buffer, sizeof(buffer));
        printf("%s\n", buffer);

        char* pwd = "pwd";
        SendData(cfd, pwd, strlen(pwd));

        memset(buffer, 0, sizeof(buffer));
        RecvData(cfd, buffer, sizeof(buffer));
        while (buffer[strlen(buffer) - 1] == '\n')
            buffer[strlen(buffer) - 1] = 0;
        printf("%s#", buffer);
        
        while (0 == 0)
        {
            fgets(buffer, sizeof(buffer), stdin);
            if (strncmp(buffer, "exit", 4) == 0)
            {
                break;
            }
            while (buffer[strlen(buffer) - 1] == '\n')
                buffer[strlen(buffer) - 1] = 0;
            SendData(cfd, buffer, strlen(buffer));
            memset(buffer, 0, sizeof(buffer));
            RecvData(cfd, buffer, sizeof(buffer));
            printf("%s\n", buffer);

            SendData(cfd, pwd, strlen(pwd));
            memset(buffer, 0, sizeof(buffer));
            RecvData(cfd, buffer, sizeof(buffer));
            while (buffer[strlen(buffer) - 1] == '\n')
                buffer[strlen(buffer) - 1] = 0;
            printf("%s#", buffer);
        }
        close(cfd);
    }
}