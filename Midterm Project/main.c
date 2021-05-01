#include "main.h"
#include "helper.h"

sem_t *sem_mutex;
sem_t *sem_full;
sem_t *sem_empty;
static char memoryName[50];
static clinic *biontech;
static process processInfo;
int main(int argc, char *argv[])
{
    
    args givenParams;
    checkArguments(argc, argv, &givenParams);
    biontech = getSharedMemory(givenParams);
    openSem();
    if (atexit(removeAll) != 0)
        errExit("atexit");
    
    /*TODO*/

    /*fork fonksiyonları yaz ve
    sinyal yaklama kodlarını ekle
    sigchild falan herşeyi dikkatli 
    adım adım anlayark yaz 13:30 sınav unutma*/

    processInfo.pid = getpid();
    processInfo.type = UNKNOWN;
    

    exit(0);
}
clinic *getSharedMemory(args givenArgs){
    struct stat sb;
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IRWXU;
    strcpy(memoryName,"clinic_sinovac344");
    int memFd = shm_open(memoryName, O_CREAT | O_RDWR, mode);
    if (memFd == -1)
        errExit("shm_open error!");
    if (fstat(memFd, &sb) == -1)
        errExit("fstat error");
    if (ftruncate(memFd, sizeof(*biontech)) == -1)  
        errExit("ftruncate error");

    clinic *gata = (clinic *)mmap(NULL, sizeof(*biontech), PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);
    if (gata == MAP_FAILED)
        errExit("mmap");
    
    gata->givenParams = givenArgs;
    gata->dose1 = 0;
    gata->dose2 = 0;
    gata->fd = safeOpen(givenArgs.iArg, O_RDWR);
    return gata;

}
void openSem(){
    /*create named semphores*/
    sem_mutex = sem_open("mutex344", O_CREAT, 0666, 1);
    if (sem_mutex == SEM_FAILED)
        errExit("sem_open error!");

    sem_full = sem_open("full344", O_CREAT, 0666, 0);
    if (sem_full == SEM_FAILED)
        errExit("sem_open error!");
    sem_empty = sem_open("empty344", O_CREAT, 0666, 10);
    if (sem_empty == SEM_FAILED)
        errExit("sem_open error!");
    

}
void removeAll()
{
    printf("hereee!!\n");
    if (sem_close(sem_full) == -1)
        errExit("sem_close");
    if (sem_close(sem_empty) == -1)
        errExit("sem_close");
    if (sem_close(sem_mutex) == -1)
        errExit("sem_close");
    sem_unlink("mutex");
    sem_unlink("full");
    sem_unlink("empty");
    shm_unlink(memoryName);
}