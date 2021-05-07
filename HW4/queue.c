#include "queue.h"

queue *createQueue()
{
    queue *head = (queue *)malloc(sizeof(queue));
    head->size = 0;
    head->rearp = NULL;
    head->frontp = NULL;
    return head;
}

void addRear(queue *head, char hw)
{
    if (head->size == 0)
    {
        head->rearp = (queue_node_t *)malloc(sizeof(queue_node_t));
        head->frontp = head->rearp;
    }
    else
    { 
        head->rearp->restp = (queue_node_t *)malloc(sizeof(queue_node_t));
        head->rearp = head->rearp->restp;
    }
    head->rearp->data = hw; 
    head->rearp->restp = NULL;
    ++(head->size);
}

char removeFront(queue *head) 
{
    queue_node_t *to_freep;         
    char ans;                       
    to_freep = head->frontp;        
    ans = to_freep->data;        
    head->frontp = to_freep->restp; 
    free(to_freep);                 
    --(head->size);
    if (head->size == 0) 
        head->rearp = NULL;
    return (ans);
}
int isQueueEmpty(queue *head){
    return (head->size == 0);
}
char getFront(queue *head){
    return head->frontp->data;
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
/*int main(int argc, char const *argv[])
{
    queue *head = createQueue();

    addRear(head, 'C');
    printf("head::%c \n", getFront(head));
    addRear(head, 'S');
    addRear(head, 'Q');
    addRear(head, 'S');
    addRear(head, 'S');
    addRear(head, 'Q');
    addRear(head, 'C');

    printQueue(head);
    
    printf("removing...\n");
    printf("%c \n", removeFront(head));
    printf("%c \n", removeFront(head));
    printf("%c \n", removeFront(head));
    printf("head::%c \n", getFront(head));
    printQueue(head);
    freeQueue(head);
    return 0;
}*/