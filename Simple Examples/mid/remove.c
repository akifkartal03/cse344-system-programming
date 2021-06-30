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

void errExit(char *msg){
    //In case of an arbitrary error, 
    //exit by printing to stderr a nicely formatted informative message.
    fprintf(stderr, "%s:%s\n",msg,strerror(errno));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{

    sem_t *sem_mutex;
    sem_t *sem_full1;
    sem_t *sem_empty1;
    sem_t *sem_full2;
    sem_t *sem_empty2;
    sem_mutex = sem_open("mutex344", O_CREAT, 0666, 1);
    if (sem_mutex == SEM_FAILED)
        errExit("sem_open error!");

    sem_full1 = sem_open("full344", O_CREAT, 0666, 0);
    if (sem_full1 == SEM_FAILED)
        errExit("sem_open error!");
    sem_empty1 = sem_open("empty344", O_CREAT, 0666, 11);
    if (sem_empty1 == SEM_FAILED)
        errExit("sem_open error!");
    sem_full2 = sem_open("full3442", O_CREAT, 0666, 0);
    if (sem_full2 == SEM_FAILED)
        errExit("sem_open error!");
    sem_empty2 = sem_open("empty3442", O_CREAT, 0666, 11);
    if (sem_empty2 == SEM_FAILED)
        errExit("sem_open error!");

    //sem_unlink("mutex344");
    //sem_unlink("full3442");
    //sem_unlink("empty3442");
    //shm_unlink("clinic_sinovac344");
    if (sem_close(sem_full1) == -1)
        errExit("sem_close");
    if (sem_close(sem_empty1) == -1)
        errExit("sem_close");
    if (sem_close(sem_full2) == -1)
        errExit("sem_close");
    if (sem_close(sem_empty2) == -1)
        errExit("sem_close");
    if (sem_close(sem_mutex) == -1)
        errExit("sem_close");
    sem_unlink("mutex344");
    sem_unlink("full344");
    sem_unlink("empty344");
    sem_unlink("full3442");
    sem_unlink("empty3442");
    shm_unlink("clinic_sinovac344");
    /*
    sem_unlink("full");
    sem_unlink("empty");
    sem_unlink("mutex");
    sem_unlink("index");
    //sem_unlink("counter");    
    shm_unlink("clinic"); 
    shm_unlink("indis"); 
    */
    return 0;

}