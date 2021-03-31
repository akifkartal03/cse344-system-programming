#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<time.h>
#include<sys/types.h> 
#include<sys/wait.h>
#include "../error/print.h"
int main(){
    pid_t pid;
    
    //create a child process
    pid = fork();

    //check return value
    if (pid < 0)
    {
        my_stderr("fork failed!\n");
    }
    else if (pid == 0)
    {
        /* child process */
        execlp("/bin/ls","ls",NULL); //run ls
    }
    else{
        /*parent process*/
        wait(NULL);
        printf("child finished!\n");
    }
    return 0;

}