#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;
    struct stat fileStat;
    if (stat(argv[1], &fileStat) < 0)
        return -1;
    printf("Information for %s\n", argv[1]);
    printf("--------------------------------------------\n");
    printf("File Size: \t\t%d bytes\n", (int)fileStat.st_size);
    printf("--------------------------------------------\n");
    if (S_ISLNK(fileStat.st_mode))
        printf("symbolic link");
    else if (S_ISDIR(fileStat.st_mode))
        printf("directory");
    else if (S_ISCHR(fileStat.st_mode))
        printf("character device");
    else if (S_ISBLK(fileStat.st_mode))
        printf("block device");
    else if (S_ISFIFO(fileStat.st_mode))
        printf("fifo");
    else if (S_ISSOCK(fileStat.st_mode))
        printf("socket");
    else if (S_ISREG(fileStat.st_mode))
        printf("regular file");
    else
        printf("error");
    printf("\n");
}