#include "print.h"

void my_printf(const char *str){
    ssize_t size = strlen(str);
    if (size != write(STDOUT_FILENO, str, size)) {
        my_stderr("write system call error\n");
    }
}
void my_stderr(const char *str){
    ssize_t size = strlen(str);
    if (size != write(STDERR_FILENO, str, size)) {
        perror("write system call error! ");
        exit(EXIT_FAILURE);
    }
    
}