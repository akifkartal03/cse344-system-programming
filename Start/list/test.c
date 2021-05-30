#include "list.h"
int main(){
    node_t *head = NULL;
    int rows = 3;
    char *cName = "test";
    char *cName1 = "test2";
    char *cName2 = "test3";
    char **c = calloc (rows,sizeof(char*));
    c[0] = "cat";
    c[1] = "dog";
    c[2] = "mouse";
    char **c1 = calloc (rows,sizeof(char*));
    c1[0] = "cat1";
    c1[1] = "dog1";
    c1[2] = "mouse1";
    char **c2 = calloc (rows,sizeof(char*));
    c2[0] = "cat2";
    c2[1] = "dog2";
    c2[2] = "mouse2";
    head = addLast(head,cName,c,rows);
    head = addLast(head,cName1,c1,rows);
    head = addLast(head,cName2,c2,rows);
    printList(head);
    printf("now freee!!\n");
    free(c);
    free(c1);
    free(c2);
    printf("afterr!\n");
    //free(cName);
    //free(cName1);
    //free(cName2);
    freeList(head);
    return 0;

}