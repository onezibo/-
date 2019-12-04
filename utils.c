#include "utils.h"

void urldecode(char *dst, const char *src)
{
    char a, b;
    while (*src)
    {
        if ((*src == '%') &&
            ((a = src[1]) && (b = src[2])) &&
            (isxdigit(a) && isxdigit(b)))
        {
            if (a >= 'a')
                a -= 'a' - 'A';
            if (a >= 'A')
                a -= ('A' - 10);
            else
                a -= '0';
            if (b >= 'a')
                b -= 'a' - 'A';
            if (b >= 'A')
                b -= ('A' - 10);
            else
                b -= '0';
            *dst++ = 16 * a + b;
            src += 3;
        }
        else if (*src == '+')
        {
            *dst++ = ' ';
            src++;
        }
        else
        {
            *dst++ = *src++;
        }
    }
    *dst++ = '\0';
}

int is_dir(char *path)
{
    // to judge a path is dir
    struct stat st;
    stat(path, &st);
    if (S_ISDIR(st.st_mode))
    {
        return 1;
    }
    return 0;
}

int has_file(char *path)
{
    // to judge if a file exists
    if (access(path, F_OK) != -1)
    {
        return 1;
    }
    return 0;
}

int file_exist(char *dir_name, char *file_name)
{
    // 1 if exists, 0 if not
    struct dirent *entry;
    struct stat statbuf;
    DIR *dir;
    dir = opendir(FILE_WORKING_DIR);
    if (dir == NULL)
    {
        printf("OpenDir error!");
        return 0;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        lstat(entry->d_name, &statbuf);
        printf("File: %s\n", entry->d_name);
        if (!S_ISCHR(statbuf.st_mode) && (strcmp(file_name, entry->d_name) == 0))
        {
            return 1;
        }
    }

    return 0;
}

int save_file(char *path, char *file_name, char *file_content, size_t file_length)
{

    char file_path[100];
    strcpy(file_path, FILE_WORKING_DIR);
    strcat(file_path, path);
    if (!has_file(file_path))
    {
        mkdir_p(file_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    strcat(file_path, "/");
    strcat(file_path, file_name);
    printf("File Path:%s", file_path);
    fflush(stdout);
    if (has_file(file_path))
    {
        return 0;
    }
    else
    {
        FILE *fp = fopen(file_path, "w+");
        printf("opened");

        if (fp == NULL)
        {
            printf("Error");
            fflush(stdout);
        }
        for (int i = 0; i < file_length; i++)
        {
            fputc(file_content[i], fp);
        }
        fclose(fp);
        return 1;
    }
}