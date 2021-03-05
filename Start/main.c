#include<stdio.h>
#include<stdlib.h>

int main(){
    int *x = malloc(sizeof(int)*10);
    x[5] = 10;
    printf("leaks try!!\n");
    free(x);
    return 0;
}