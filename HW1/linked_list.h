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


//regex information node
typedef struct node_s
{
    int position;
    char preChr;
    struct node_s *next;

} node_t;

extern volatile __sig_atomic_t exitSignal;
node_t *addFirst(node_t *head, char prevChar, int position);
node_t *addLast(node_t *head, char prevChar, int position);
void freeList(node_t *head);
void exitHandler(int signal);



#endif