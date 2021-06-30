#ifndef MY_LINKEDLIST_H
#define MY_LINKEDLIST_H
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


//column information node
typedef struct node_s
{
    int size;
    int capacity;
    char *columnName;
    char **data;
    struct node_s *next;

} node_t;


node_t *addFirst(node_t *head, char *cName, int s,int c);
node_t *addLast(node_t *head, char *cName, int s,int c);
node_t *find(node_t *head, char *cName);
node_t *findByIndex(node_t *head, int index);
void set(node_t *head, char *cName, int index, char *newData);
void freeList(node_t *head);
void printList(node_t *head);
int isContain(node_t *node,char *element);


#endif