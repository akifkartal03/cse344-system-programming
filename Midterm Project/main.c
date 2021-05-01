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
    
    /*TODO*/

    /*fork fonksiyonları yaz ve
    sinyal yaklama kodlarını ekle
    sigchild falan herşeyi dikkatli 
    adım adım anlayark yaz 13:30 sınav unutma*/





    processInfo.pid = getpid();
    processInfo.type = PARENT;
    processInfo.index = 0;
    

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
void createNurses(clinic *biontech){
   
    for (int i = 0; i < biontech->givenParams.nArg; i++)
    {
        
        if (fork() == 0)
        {
            /* child process*/

            processInfo.pid = getpid();
            processInfo.type = NURSE;
            processInfo.index = i;
            createSignalHandler();
            nurse(biontech,&processInfo);
            cleanAndExit();
        }
        
    }
    
}
void createVaccinators(clinic *biontech){
    for (int i = 0; i < biontech->givenParams.vArg; i++)
    {
        
        if (fork() == 0)
        {
            /* child process*/

            processInfo.pid = getpid();
            processInfo.type = VACCINATOR;
            processInfo.index = i;
            createSignalHandler();
            vaccinator(biontech,&processInfo);
            cleanAndExit();
        }
        
    }
}
void createCitizens(clinic *biontech){
    for (int i = 0; i < biontech->givenParams.vArg; i++)
    {
        
        if (fork() == 0)
        {
            /* child process*/

            processInfo.pid = getpid();
            processInfo.type = CITIZEN;
            processInfo.index = i;
            createSignalHandler();
            citizen(biontech,&processInfo);
            cleanAndExit();
        }
        
    }
}
void createSignalHandler(){
    struct sigaction sa;
    sa.sa_handler = exitHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        errExit("sigaction error!");
    }
}
void exitHandler(int signal)
{
    if (signal == SIGINT)
    {
        int savedErrno = errno;
        cleanAndExit();
        errno = savedErrno;
        
    }
}
void cleanAndExit(){
    
    if (processInfo.type == PARENT)
    {
        reapDeadChildren();
        removeAll();
        if (close(biontech->fd) < 0)
        {
            errExit("close file error!");
        }
    }
    int unmap = munmap(biontech,sizeof(*biontech));
    if (unmap == -1)
    {
        errExit("unmap error!");
    }
    exit(EXIT_SUCCESS);
}
void reapDeadChildren(){
    pid_t childPid;
    int status;
    while ((childPid = waitpid(-1, &status, 0)) > 0);
    if (childPid == -1 && errno != ECHILD)
        errExit("waitpid"); 
}

void nurse(clinic *biontech, process *process){

}
void vaccinator(clinic *biontech, process *process);
void citizen(clinic *biontech, process *process);    