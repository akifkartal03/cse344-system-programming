#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

enum HwType{
    S,
    C,
    Q
};

typedef struct node_s
{
    enum HwType hw;
    struct node_s *next;

} queue;

//queue *head;


queue *addLast(queue *head, enum HwType data);
queue* removeFront(queue *head);
int isEmpty(queue *head);
void freeQueue(queue *head);

#endif