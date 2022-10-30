#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    system("ifconfig > ifconfig.out");
    FILE* f = fopen("ifconfig.out", "rb");
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* data = (char*)calloc(size + 1, sizeof(char));
    fread(data, sizeof(char), size, f);
    char* tmp = data;
    while (strstr(tmp, "flags=") != NULL)
    {
        tmp = strstr(tmp, "flags=");
        while (tmp != data && tmp[0] != '\n')
        {
            tmp -= 1;
        }
        if (tmp[0] == '\n')
        {
            tmp += 1;
        }
        char adapter[1024] = { 0 };
        char inet[1024] = { 0 };
        char ip4[1024] = { 0 };
        char ip6[1024] = { 0 };
        sscanf(tmp, "%s", adapter);
        if (strstr(tmp, "inet") != NULL)
        {
            tmp = strstr(tmp, "inet");
            sscanf(tmp, "%s%s", inet, ip4);
        }
        if (strstr(tmp, "inet6") != NULL)
        {
            tmp = strstr(tmp, "inet6");
            sscanf(tmp, "%s%s", inet, ip6);
        }
        printf("%s ", adapter);
        if (strlen(ip4) > 0)
        {
            printf("%s ", ip4);
        }else
            printf("IPv4 Not Found ");

        if (strlen(ip6) > 0)
        {
            printf("%s ", ip6);
        }else
            printf("IPv6 Not Found ");
        printf("\n");
    }
    free(data);
    data = NULL;
    fclose(f);
}
