#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    system("ls -al > ls.out");
    FILE* f = fopen("ls.out", "rt");
    char line[1024];
    while (!feof(f))
    {
        memset(line, 0, sizeof(line));
        fgets(line, sizeof(line), f);
        if (line[0] == '-')
        {
            char per[1024] = { 0 };
            char v1[1024] = { 0 };
            char group[1024] = { 0 };
            char owner[1024] = { 0 };
            char size[1024] = { 0 };
            char month[1024] = { 0 };
            char data[1024] = { 0 };
            char time[1024] = { 0 };
            sscanf(line, "%s%s%s%s%s%s%s%s", per, v1, group, owner, size, month, data, time);
            char* name = strstr(line, time) + strlen(time) + 1;
            while ( name[strlen(name) - 1] == '\n' ||
                    name[strlen(name) - 1] == '\r')
            {
                name[strlen(name) - 1] = 0; //'\0'
            }
            printf("%s %s\n", name, size);
        }
    }
    fclose(f);
}