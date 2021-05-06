#include "queue.h"

queue *addLast(queue *head, enum HwType data){
    if (head == NULL)
    {
        queue *temp = (queue *)malloc(sizeof(queue));
        temp->hw = data;
        temp->next = head;
        head = temp;
        return head;
    }
    queue *iter;
    iter = head;
    while (iter->next != NULL)
    {
        iter = iter->next;
    }
    queue *temp = (queue *)malloc(sizeof(queue));
    temp->hw = data;
    iter->next = temp;
    temp->next = NULL;
    return head;
}
queue* removeFront(queue *head){
    queue *temp = head;
    head = head->next;
    free(temp);
    return head;
}
int isEmpty(queue *head){
    return (head == NULL);
}
void freeQueue(queue *head){
    queue *iter;
    while (head != NULL)
    {
        iter = head;
        head = head->next;
        free(iter);
    }  
}
/*int main(int argc, char const *argv[])
{
    queue *head = NULL;
    //printf("%d\n",isListEmpty(head));
    for (int i = 1; i < 6; i++)
    {
        enum HwType h = C;
        
        head = addLast(head,h);
    }
    head = removeFront(head);
    head = removeFront(head);
    head = removeFront(head);
    head = removeFront(head);
    //head = removeFront(head);
    queue *iter = head;
    while (iter!=NULL)
    {
        switch (iter->hw)
        {
        case S:
            printf("S\n");
            break;
        case C:
            printf("C\n");
            break;
        case Q:
            printf("Q\n");
            break;
        default:
            break;
        }
        iter= iter->next;
    }
    
    
    printf("%d\n",isEmpty(head));
    freeQueue(head);
    return 0;
}*/