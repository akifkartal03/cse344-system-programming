#ifndef MY_LINKEDLIST_H
#define MY_LINKEDLIST_H
#include<stdio.h>
#include<stdlib.h>

typedef struct node_s
{
    int position;
    char preChr;
    struct node_s *next;

}node_t;


node_t* addFirst(node_t *head , char prevChar,int position);
node_t* addLast(node_t *head , char prevChar,int position);

#endif