#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void create_process(int x, char** args){
    int pid;
    if ((pid = fork()) == -1){
        fprintf(stderr, "Can't fork.");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0){

        printf("%s (%d) has just started...\n", args[0], x);
        execvp(args[0], args);
        exit(EXIT_SUCCESS);
    }

}

int main(void){

   
    char *args1[] = {"./player", "-b", "0", "-s", "mymemory", "-f", "/home/akif/Desktop/source/example", "-m","sem1"};
    char *args2[] = {"./player", "-b", "3", "-s", "mymemory", "-f", "/home/akif/Desktop/source/example", "-m", "sem1"};
    char *args3[] = {"./player", "-b", "4", "-s", "mymemory", "-f", "/home/akif/Desktop/source/example", "-m","sem1"};

    create_process(1, args1);
    create_process(2, args2);
    create_process(3, args3);

    return 0;
}
