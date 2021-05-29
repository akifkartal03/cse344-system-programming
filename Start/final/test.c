#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <errno.h>
#include <semaphore.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>

int main(int argc, char *argv[])
{
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IRWXU;
    int fd = open(argv[1], O_CREAT | O_WRONLY, mode);
    char buffer[25] = "this is\na test file.";
    write(fd,buffer,20);
    return 0;
}
