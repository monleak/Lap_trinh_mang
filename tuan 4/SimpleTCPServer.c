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
int main()
{
    int sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN saddr, caddr;
    int clen = sizeof(caddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5555);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int error = bind(sfd, (SOCKADDR*)&saddr, sizeof(saddr));
    if (error != -1)
    {
        listen(sfd, 10);
        int cfd = accept(sfd, (SOCKADDR*)&caddr, &clen);
        if (cfd != INVALID_SOCKET)
        {
            char* welcome = "Welcome to my first TCP server\n";
            send(cfd, welcome, strlen(welcome), 0);
            while (0 == 0)
            {
                char buffer[1024] = { 0 };
                recv(cfd, buffer, sizeof(buffer), 0);
                printf("%s\n", buffer);
                if (strncmp(buffer, "exit", 4) == 0)
                {
                    break;
                }
            }
            close(cfd);
        }
    }else
        printf("Failed to bind\n");
    close(sfd);
}