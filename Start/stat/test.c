#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
int main(int argc, char **argv)
{
    if (argc != 2)
        return 1;
    struct stat sfile;
    if (stat(argv[1], &sfile) < 0)
        return -1;
    printf("\n");
    printf((sfile.st_mode & S_IRUSR)? "r":"-");
    printf((sfile.st_mode & S_IWUSR)? "w":"-");
    printf((sfile.st_mode & S_IXUSR)? "x":"-");
    printf((sfile.st_mode & S_IRGRP)? "r":"-");
    printf((sfile.st_mode & S_IWGRP)? "w":"-");
    printf((sfile.st_mode & S_IXGRP)? "x":"-");
    printf((sfile.st_mode & S_IROTH)? "r":"-");
    printf((sfile.st_mode & S_IWOTH)? "w":"-");
    printf((sfile.st_mode & S_IXOTH)? "x":"-");
    printf("\n");
    printf("\n");
    printf("link number: %d\n",(int)sfile.st_nlink);
}