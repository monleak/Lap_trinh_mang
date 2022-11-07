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

#define INVALID_SOCKET -1
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

void signal_handler(int signum)
{
    printf("Signal: %d\n", signum);
    if (signum == SIGINT)
    {
        exit(0);
    }
}

int main()
{
    signal(SIGINT, signal_handler);
    signal(SIGCHLD, signal_handler);

    if (fork() == 0)
    {
        printf("I am in a child process\n");
    }else
    {
        if (fork() == 0)
        {
            printf("I am in a child process\n");
        }else
        {
            if (fork() == 0)
            {
                printf("I am in a child process\n");
            }
        }
    }
    printf("Hello\n");
    sleep(10);
    return 0;
}