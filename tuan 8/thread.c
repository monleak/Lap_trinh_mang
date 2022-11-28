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
void* my_func(void* arg)
{
    int a = *(int*)arg;
    printf("Hello from thread: %d\n", a);
    return NULL;
}
int a1 = 1;
int a2 = 2;
int a3 = 3;
int a4 = 4;
int a5 = 5;
int main()
{
    pthread_t tid1, tid2, tid3, tid4, tid5;
    pthread_create(&tid1, NULL, my_func, &a1);
    pthread_create(&tid2, NULL, my_func, &a2);
    pthread_create(&tid3, NULL, my_func, &a3);
    pthread_create(&tid4, NULL, my_func, &a4);
    pthread_create(&tid5, NULL, my_func, &a5);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    pthread_join(tid4, NULL);
    pthread_join(tid5, NULL);

    printf("Hello from main\n");
}