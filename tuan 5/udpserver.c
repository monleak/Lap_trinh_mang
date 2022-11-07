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
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (s == INVALID_SOCKET)
    {
        perror("socket");
    }
    SOCKADDR_IN saddr, caddr;
    int clen = sizeof(caddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5000);
    saddr.sin_addr.s_addr = INADDR_ANY;
    bind(s, (SOCKADDR *)&saddr, sizeof(saddr));
    while (1)
    {
        // Receive from server
        char buffer[1024] = {0};
        int r = recvfrom(s, buffer, sizeof(buffer), 0, (SOCKADDR *)&caddr, &clen);
        if (r > 0)
        {
            caddr.sin_port = htons(6000);
            sendto(s, buffer, strlen(buffer), 0, (SOCKADDR *)&caddr, clen);
        }
        else
            break;
    }

    close(s);
    return 0;
}