#include "linked_list.h"
volatile __sig_atomic_t exitSignal = 0;
node_t *addFirst(node_t *head, char prevChar, int position)
{
    node_t *temp = (node_t *)malloc(sizeof(node_t));
    temp->position = position;
    temp->preChr = prevChar;
    temp->next = head;
    head = temp;
    if (exitSignal)
    {
        printf("You are exiting...\n");
        freeList(head);
        exit(EXIT_SUCCESS);
    }
    
    return head;
}
node_t *addLast(node_t *head, char prevChar, int position)
{
    if (head == NULL)
    {
        return addFirst(head, prevChar, position);
    }
    node_t *iter;
    iter = head;
    while (iter->next != NULL)
    {
        iter = iter->next;
    }
    node_t *temp = (node_t *)malloc(sizeof(node_t));
    temp->position = position;
    temp->preChr = prevChar;
    iter->next = temp;
    temp->next = NULL;
    if (exitSignal)
    {
        printf("You are exiting...\n");
        freeList(head);
        exit(EXIT_SUCCESS);
    }
    return head;
}
void freeList(node_t *head)
{
    node_t *iter;
    while (head != NULL)
    {
        iter = head;
        head = head->next;
        free(iter);
    }    
}
void exitHandler(int signal)
{
    if (signal == SIGINT)
    {
        exitSignal = 1;
    }
}
