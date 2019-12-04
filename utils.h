#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>
#include <event.h>
#include <dirent.h>
#include <sys/stat.h>
#include "config.h"
void urldecode(char *dst, const char *src);
int is_dir(char *path);
int has_file(char *path);
int file_exist(char *dir_name, char *file_name);
char *strstr2(const char *s1, const char *s2, size_t len1);
int mkdir_p(char *dir, mode_t m)