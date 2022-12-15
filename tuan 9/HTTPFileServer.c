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
#define MAX_CLIENT_COUNT 1024
int g_cfd[MAX_CLIENT_COUNT] = { 0 };
int g_count = 0;

void Append(char** psrc, const char* dest)
{
    char* src = *psrc;
    int oldLen = src == NULL ? 0 : strlen(src);
    *psrc = (char*)realloc(*psrc, oldLen + strlen(dest) + 1);
    src = *psrc;
    memset(src + oldLen, 0, strlen(dest) + 1);
    sprintf(src + oldLen, "%s", dest);
}

int Compare(const struct dirent** A, const struct dirent** B)
{
    if ((*A)->d_type == (*B)->d_type)
    {
        return 0;
    }else if ((*A)->d_type == DT_DIR)
    {
        return -1;
    }else
        return 1;
}

void* ClientThread(void* arg)
{
    int cfd = *((int*)arg);
    free(arg);
    arg = NULL;
    char buffer[1024] = { 0 };
    recv(cfd, buffer, sizeof(buffer), 0);
    char GETPOST[8] = { 0 };
    char PATH[1024] = { 0 };
    sscanf(buffer, "%s%s", GETPOST, PATH);
    while (strstr(PATH, "%20") != NULL)
    {
        char* tmp = strstr(PATH, "%20");
        tmp[0] = ' ';
        strcpy(tmp + 1, tmp + 3);
    }
    printf("%s\n", PATH);
    struct dirent** output = NULL;
    char* rootPath = (char*)calloc(1024, 1);
    strcpy(rootPath, "/mnt/e");
    Append(&rootPath, PATH);
    if (PATH[strlen(PATH) - 1] == '/')
    {
        char* ok = "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: text/html\r\n\r\n";
        char* html = NULL;
        Append(&html, "<html>");
        int n = scandir(rootPath, &output, NULL, Compare);
        if (n > 0)
        {
            for (int i = 0;i < n;i++)
            {
                if (output[i]->d_type == DT_REG)
                {
                    char tmp[2048] = { 0 };
                    if (PATH[strlen(PATH) - 1] == '/')
                        sprintf(tmp, "<a href=\"%s%s\"><i>%s</i></a><br>",PATH, output[i]->d_name, output[i]->d_name);
                    else
                        sprintf(tmp, "<a href=\"%s/%s\"><i>%s</i></a><br>",PATH, output[i]->d_name, output[i]->d_name);
                    Append(&html, tmp);
                }
                if (output[i]->d_type == DT_DIR)
                {
                    char tmp[2048] = { 0 };
                    if (PATH[strlen(PATH) - 1] == '/')
                        sprintf(tmp, "<a href=\"%s%s/\"><b>%s</b></a><br>",PATH, output[i]->d_name, output[i]->d_name);
                    else
                        sprintf(tmp, "<a href=\"%s/%s/\"><b>%s</b></a><br>",PATH, output[i]->d_name, output[i]->d_name);
                    Append(&html, tmp);
                }
            }
        }else{
            Append(&html,"<b>EMPTY FOLDER</b>");
        }

        Append(&html, "</html>");
        char header[1024] = { 0 };
        sprintf(header, ok, strlen(html));
        send(cfd, header, strlen(header), 0);
        send(cfd, html, strlen(html), 0);
        close(cfd);

        free(html);
        html = NULL;
    }else
    {
        char* ok = "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: %s\r\n\r\n";
        char* notfound = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nContent-Type: text/html\r\n\r\n";
        FILE* f = fopen(rootPath, "rb");
        if (f != NULL)
        {
            fseek(f, 0, SEEK_END);
            int size = ftell(f);
            fseek(f, 0, SEEK_SET);
            char* data = (char*)calloc(size, 1);
            fread(data, 1, size, f);
            fclose(f);
            char header[1024];
            int i = strlen(PATH) - 1;
            while (i >= 0 && PATH[i] != '.') i--;

            if (i >= 0 && strcmp(PATH + i, ".c") == 0)
            {
                sprintf(header, ok, size, "text/plain");
            }else if (i >= 0 && strcmp(PATH + i, ".jpg") == 0)
            {
                sprintf(header, ok, size, "image/jpg");
            }else if (i >= 0 && strcmp(PATH + i, ".mp4") == 0)
            {
                sprintf(header, ok, size, "video/mp4");
            }else
                sprintf(header, ok, size, "application/octet-stream");

            send(cfd, header, strlen(header), 0);
            int sent = 0;
            while (sent < size)
            {
                int tmp = send(cfd, data + sent, size - sent, 0);
                sent += tmp;
            }
            free(data);
            data = NULL;
        }else
        {
            send(cfd, notfound, strlen(notfound), 0);
        }

        close(cfd);
    }
    free(rootPath);
    rootPath = NULL;

    return NULL;
}
int main()
{
    int sfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN saddr, caddr;
    int clen = sizeof(caddr);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8888);
    saddr.sin_addr.s_addr = 0;
    if (bind(sfd, (SOCKADDR*)&saddr, sizeof(saddr)) == 0)
    {
        listen(sfd, 10);
        while (0 == 0)
        {
            int tmp = accept(sfd, (SOCKADDR*)&caddr, &clen);
            if (tmp != INVALID_SOCKET)
            {
                g_cfd[g_count++] = tmp;
                pthread_t tid = 0;
                int* arg = (int*)calloc(1, sizeof(int));
                *arg = tmp;
                pthread_create(&tid, NULL, ClientThread, (void*)arg);
            }
        }
    }else
        printf("PORT not available\n");
}