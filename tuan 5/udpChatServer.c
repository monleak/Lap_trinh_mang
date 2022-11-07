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
SOCKADDR_IN* listClient = NULL;
int countClient = 0;

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
            printf("%s\n", buffer);
            caddr.sin_port = htons(7000);
            if (strncmp(buffer, "REG", 3) == 0)
            {
                listClient = (SOCKADDR_IN*)realloc(listClient, (countClient + 1) * sizeof(SOCKADDR));
                listClient[countClient++] = caddr;
            }else if (strncmp(buffer, "CHAT", 4) == 0)
            {
                for (int i = 0;i < countClient;i++)
                {
                    sendto(s, buffer, strlen(buffer), 0, (SOCKADDR*)&(listClient[i]), sizeof(SOCKADDR));
                }
            }
        }
        else
            break;
    }

    close(s);
    free(listClient);
    listClient = NULL;
    return 0;
}