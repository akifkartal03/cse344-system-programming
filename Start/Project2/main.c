#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<time.h>
#include<sys/types.h> 
#include<string.h>
#include<sys/wait.h>

#define TRUE 1

int main(int argc, char *argv[]){
    int status;
    while (TRUE) { /* repeat forever */
        type_prompt(); /* display prompt on the screen */
        read_command(argv[0], argv); /* read input from terminal */
        int pid = fork();
        if (pid != 0) { /* fork off child process */
            /* Parent code. */
            waitpid(pid, &status, 0); /* wait for child to exit */
        } else {
        /* Child code. */
            execve(argv[0], argv, 0); /* execute command */
        }
    }
    return 0;
}