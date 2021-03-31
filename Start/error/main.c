#include "print.h"

int main(){

    my_printf("printff\n");
    my_stderr("stderrr\n");
    perror("write system call error!");
    return 0;
}