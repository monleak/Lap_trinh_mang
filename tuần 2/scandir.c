#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

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

int main()
{
    char* html = NULL;
    Append(&html, "<html>");
    char path[1024] = { 0 };
    char root[1024] = { 0 };
    struct dirent** output = NULL;
    strcpy(path, "/home/monleak/Lap_trinh_mang");
    strcpy(root, "/home/monleak/Lap_trinh_mang");
    int exit = 0;
    do
    {
        int n = scandir(path, &output, NULL, Compare);
        if (output != NULL)
        {
            for (int i = 0;i < n;i++)
            {
                if (output[i]->d_type == DT_REG)
                {
                    char tmp[1024] = { 0 };
                    sprintf(tmp, "<a href=\"%s\"><i>%s</i></a><br>", output[i]->d_name, output[i]->d_name);
                    Append(&html, tmp);
                }
                if (output[i]->d_type == DT_DIR)
                {
                    char tmp[1024] = { 0 };
                    sprintf(tmp, "<a href=\"%s\"><b>%s</b></a><br>", output[i]->d_name, output[i]->d_name);
                    Append(&html, tmp);
                }
            }
            Append(&html, "</html>");
            FILE* f = fopen("scandir.html","w");
            fprintf(f, "%s", html);
            fclose(f);
            free(html);
            html = NULL;

            char command[1024] = { 0 };
            fgets(command, sizeof(command), stdin);
            while ( command[strlen(command) - 1] == '\r' ||
                    command[strlen(command) - 1] == '\n')
            {
                command[strlen(command) - 1] = 0;
            }

            exit = 1;
            if (strcmp(command, "..") == 0)
            {
                exit = 0;
                if (strcmp(path, root) != 0)
                {
                    path[strlen(path) - 1] = 0;
                    while (path[strlen(path) - 1] != '/')
                    {
                        path[strlen(path) - 1] = 0;
                    }
                }
            }else
            {
                for (int i = 0;i < n;i++)
                {
                    if ((output[i]->d_type == DT_DIR) &&
                        (strcmp(output[i]->d_name, command) == 0))
                    {
                        exit = 0;
                        sprintf(path + strlen(path), "%s/", command);
                        break;
                    }
                }
            }
            for (int i = 0;i < n;i++)
            {
                free(output[i]);
                output[i] = NULL;
            }
            free(output);
            output = NULL;
        }else
            exit = 1;
    }while (!exit);
}