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

#define INVALID_SOCKET -1
#define MAX_CLIENT 1024
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;

typedef struct
{
    int start;
    int stop;
} ARG;

pthread_mutex_t* mutex = NULL;
int n, k;
int sum = 0;
int m;
void* my_func(void* arg)
{
    int start = (*(ARG*)arg).start;
    int stop = (*(ARG*)arg).stop;
    for (int i = start;i <= stop;i++)
    {
        pthread_mutex_lock(mutex);
        sum += i;
        pthread_mutex_unlock(mutex);
    }
    free(arg);
    arg = NULL;
    return NULL;
}

int main()
{
    mutex = (pthread_mutex_t*)calloc(1, sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex, NULL);

    int n,k;
    do{
        printf("Nhap n,k: ");
        scanf("%d %d",&n,&k);
    }while(n>k || n==0);
    
    pthread_t* tid = (pthread_t*)calloc(n, sizeof(pthread_t));
    for (int i = 0;i < n;i++)
    {
        ARG* arg = (ARG*)calloc(1, sizeof(ARG));
        arg->start = i * m + 1;
        if (i < n - 1)
            arg->stop = arg->start + m - 1;
        else
            arg->stop = k;
        pthread_create(&tid[i], NULL, my_func, arg);
    }
    for (int i = 0;i < n;i++)
    {
        pthread_join(tid[i], NULL);
    }
    printf("Sum = %d\n", sum);
    free(tid);
    tid = NULL;
    pthread_mutex_destroy(mutex);
    free(mutex);
}