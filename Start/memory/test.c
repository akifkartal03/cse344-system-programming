#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/mman.h>

typedef struct potato
{
    pid_t pot_pid;
    int switches;
    char fifo_name[50];
    int done;
}player;
void errExit(char *msg){
    //In case of an arbitrary error, 
    //exit by printing to stderr a nicely formatted informative message.
    fprintf(stderr, "%s:%s\n",msg,strerror(errno));
    exit(EXIT_FAILURE);
}
int main(int argc, char *argv[]){
    int start = atoi(argv[1]);
    int fd;
    struct stat sb;
    player *data;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IRWXU;
    sem_t *sem_id = sem_open("test", O_CREAT, 0666, 1);
    fd = shm_open("myname", O_CREAT | O_RDWR, mode);
    if (fd == -1)
        errExit("shm_open error!");
    if (fstat(fd, &sb) == -1)
        errExit("fstat error");
    size_t len = 4096;
    if (sb.st_size == 0)
    {
        if (ftruncate(fd, len) == -1)
            errExit("ftruncate error");
    }
    data = (player *)mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED)
        errExit("mmap");
    int res = 0, k = 1;
    
    if (sem_wait(sem_id) == -1)
        errExit("sem_wait");
    printf("start:%d\n",start);
    printf("run pid:%ld\n",(long)getpid());
    for (int i = 0; i < start+5; ++i)
    {
    
       printf("switch:%d\n",data[i].switches);
       printf("pid:%ld\n",(long)data[i].pot_pid);
       printf("name:%s\n",data[i].fifo_name);
        
    }
    for (int i = start; i < start+5; ++i)
    {
        
        player info;
        strcpy(info.fifo_name, "akif");
        info.switches = i;
        info.pot_pid = getpid();
        info.done = 1;
        memcpy(&data[i], &info, sizeof(info));
        
    }
    
    if (sem_post(sem_id) == -1)
        errExit("sem_post");
    exit(EXIT_SUCCESS);
}