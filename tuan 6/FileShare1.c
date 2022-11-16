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
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define INVALID_SOCKET -1
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

void signal_handler(int signum)
{
    if (signum == SIGCHLD)
    {
        int stat = 0;
        while (waitpid(-1, &stat, WNOHANG) > 0)
        {

        }
    }
}

int main()
{
    int parent_id = getpid();
    signal(SIGCHLD, signal_handler);
    SOCKADDR_IN saddr1, saddr2, caddr;
    int clen = sizeof(caddr);
    saddr1.sin_family = AF_INET;
    saddr1.sin_port = htons(5000);
    saddr1.sin_addr.s_addr = 0;
    int tcp_sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    bind(tcp_sfd, (SOCKADDR*)&saddr1, sizeof(saddr1));
    listen(tcp_sfd, 10);
    int udp_sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    saddr2.sin_family = AF_INET;
    saddr2.sin_port = htons(4000);
    saddr2.sin_addr.s_addr = 0;
    bind(udp_sfd, (SOCKADDR*)&saddr2, sizeof(saddr2));
    if (fork() == 0)
    {
        while (0 == 0)
        {
            char buffer[1024] = { 0 };
            printf("Waiting for broadcasted data...\n");
            int r = recvfrom(udp_sfd, buffer, sizeof(buffer), 0, (SOCKADDR*)&caddr, &clen);
            if (r > 0)
            {
                while (buffer[strlen(buffer) - 1] == '\n' || buffer[strlen(buffer) - 1] == '\r')
                {
                    buffer[strlen(buffer) - 1] = 0;
                }
                
                printf("Received from UDP4000: %s\n", buffer);
                FILE* f = fopen("client.txt","at");
                fprintf(f, "%s %s\n", buffer, inet_ntoa(caddr.sin_addr));
                fclose(f);
            }else
                printf("Failed to recvfrom\n");
        }
    }else
    {
        if (fork() == 0)
        {

        }else
        {
            while (0 == 0)
            {
                sleep(1000);
            }
        }
    }
    return 0;
}