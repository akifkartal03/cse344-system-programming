#include "c_queue.h"

queue *createQueue()
{
    queue *head = (queue *)malloc(sizeof(queue));
    head->size = 0;
    head->rearp = NULL;
    head->frontp = NULL;
    return head;
}

void add_to_q(queue *head, char hw)
{
    if (head->size == 0)
    { /* adds to empty queue */
        head->rearp = (queue_node_t *)malloc(sizeof(queue_node_t));
        head->frontp = head->rearp;
    }
    else
    { /* adds to nonempty queue */
        head->rearp->restp = (queue_node_t *)malloc(sizeof(queue_node_t));
        head->rearp = head->rearp->restp;
    }
    head->rearp->data = hw; /* defines newly added node */
    head->rearp->restp = NULL;
    ++(head->size);
}
/*
* Removes and frees first node of queue, returning value stored there.
* Pre: queue is not empty
*/
char remove_from_q(queue *head) /* input/output - queue */
{
    queue_node_t *to_freep;         /* pointer to node removed */
    char ans;                       /* initial queue value which is to be returned */
    to_freep = head->frontp;        /* saves pointer to node being deleted */
    ans = to_freep->data;        /* retrieves value to return */
    head->frontp = to_freep->restp; /* deletes first node */
    free(to_freep);                 /* deallocates space */
    --(head->size);
    if (head->size == 0) /* queue's ONLY node was deleted */
        head->rearp = NULL;
    return (ans);
}
int isQueueEmpty(queue *head){
    return (head->size == 0);
}
void printQueue(queue *head)
{
    queue_node_t *iter = head->frontp;
    for (int i = 0; i < head->size; i++)
    {
        printf("%c ", iter->data);
        iter = iter->restp;
    }
    printf("\n");
}
void freeQueue(queue *head)
{

    /*for (int i = 0; i < head->size; i++)
    {
        queue_node_t *temp = head->frontp;
        head->frontp = head->frontp->restp;
        free(temp);
    }*/
    while (head->frontp != NULL)
    {
        queue_node_t *temp = head->frontp;
        head->frontp = head->frontp->restp;
        free(temp);
    }

    //free(head->rearp);
    free(head);
}
int main(int argc, char const *argv[])
{
    queue *head = createQueue();

    add_to_q(head, 'C');
    add_to_q(head, 'S');
    add_to_q(head, 'Q');
    add_to_q(head, 'S');
    add_to_q(head, 'S');
    add_to_q(head, 'Q');
    add_to_q(head, 'C');

    printQueue(head);
    
    printf("removing...\n");
    printf("%c \n", remove_from_q(head));
    printf("%c \n", remove_from_q(head));
    printf("%c \n", remove_from_q(head));

    printQueue(head);
    freeQueue(head);
    return 0;
}