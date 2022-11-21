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
        int cfd = accept(sfd, (SOCKADDR*)&caddr, &clen); //chấp nhận kết nối
        if (cfd != INVALID_SOCKET)
        {
            char* welcome = "Welcome to my first TCP server\n"; //Gửi lời chào đến client
            SendData(cfd, welcome, strlen(welcome));

            while (0 == 0)
            {
                char buffer[1000] = { 0 };
                RecvData(cfd, buffer, sizeof(buffer));

                if (strncmp(buffer, "exit", 4) == 0)
                {
                    break;
                }

                while (buffer[strlen(buffer) - 1] == '\n')
                    buffer[strlen(buffer) - 1] = 0;
                char command[1024] = { 0 };
                if (strncmp(buffer, "cd", 2) == 0)
                {
                    char* result = "Directory Changed\n";
                    chdir(buffer + 3);
                    SendData(cfd, result, strlen(result));
                }else
                {
                    sprintf(command, "%s > out.txt", buffer);
                    system(command); //Shell Process, cd .. ->Shell Process
                    FILE* f = fopen("out.txt", "rb");
                    if (f != NULL)
                    {
                        fseek(f, 0, SEEK_END);
                        int fsize = ftell(f);
                        fseek(f, 0, SEEK_SET);
                        char* data = (char*)calloc(fsize, 1);
                        fread(data, 1, fsize, f);
                        fclose(f);
                        SendData(cfd, data, fsize);
                        free(data);
                        data = NULL;
                    }else
                    {
                        char* result = "Permission Denied\nCannot create out.txt in the current folder\n";
                        SendData(cfd, result, strlen(result));
                    }
                }
            }
            close(cfd);
        }
    }else
        printf("Failed to bind\n");
    close(sfd);
}