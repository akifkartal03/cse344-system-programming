#ifndef _LINKED_LIST_H_
#define _LINKED_LIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>



typedef struct node_s
{
    int data;
    struct node_s *next;

} node_t;

//node_t *head;

node_t *addFirst(node_t *head, int data);
node_t *addLast(node_t *head, int data);
int isListEmpty(node_t *head);
node_t *findNode(node_t *head, int data);
void removeNode(node_t *head, int data);
node_t *findPrevNode(node_t *head, int data);
void freeList(node_t *head);
void printList(node_t *head);
int indexOf(node_t *head, int data);
void removeIndex(node_t *head,int index);
node_t* removeFront(node_t *head);


#endif