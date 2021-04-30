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

int main(int argc, char **argv)
{
    
    sem_unlink("full");
    sem_unlink("empty");
    sem_unlink("mutex");
    sem_unlink("index");
    //sem_unlink("counter");    
    shm_unlink("clinic"); 
    shm_unlink("indis"); 
    return 0;

}