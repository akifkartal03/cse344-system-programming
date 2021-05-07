#ifndef _LINKED_LIST_QUEUE_H_
#define _LINKED_LIST_QUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

typedef struct queue_node_s
{
    char data;
    struct queue_node_s *restp;
} queue_node_t;

typedef struct node
{
    queue_node_t *frontp;
    queue_node_t *rearp;
    int size;
} queue;

queue *createQueue();
void addRear(queue *head, char hw);
char removeFront(queue *head);
int isQueueEmpty(queue *head);
void printQueue(queue *head);
void freeQueue(queue *head);

#endif