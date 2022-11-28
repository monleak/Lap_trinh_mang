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
#include <pthread.h>

#define MAX_CONN_NUM 1024
#define INVALID_SOCKET -1
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

int g_cfd[MAX_CONN_NUM];
pthread_t g_tid[MAX_CONN_NUM];
int g_count = 0;
int exitflag = 0;
int sfd = 0;

void* myfunc(void*arg){  
    int cfd = *(int*) arg;  
    while (exitflag == 0)
    {
        printf("Waiting for data...\n");
        char buffer[1024] = { 0 };
        int r = recv(cfd, buffer, sizeof(buffer), 0);
        if (r > 0)
        {
            for(int i=0; i<g_count; i++){
                if(g_cfd[i] != cfd){
                    send(g_cfd[i], buffer, strlen(buffer), 0);
                }
            }
        }else
        {
            printf("A client has disconnected\n");
            break;
        }
    }  
    close(cfd);      
} 

void sighandler(int signum)
{
    exitflag = 1;
    close(sfd);
    for (int i = 0;i < g_count;i++)
    {
        close(g_cfd[i]);
    }
    printf("exitflag=1\n");
}

int main()
{
    signal(SIGINT, sighandler);
    SOCKADDR_IN saddr, caddr;
    int clen = sizeof(caddr);
    pthread_t tid1;

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(5000);
    saddr.sin_addr.s_addr = 0;
    sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    bind(sfd, (SOCKADDR*)&saddr, sizeof(saddr));
    listen(sfd, 10);

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

    return 0;
}