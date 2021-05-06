#include "linked_list.h"

node_t *createList(){
    node_t *ptr = malloc(sizeof(node_t));
	ptr->next = NULL;
	return ptr;
}

node_t *addFirst(node_t *head, int data)
{
    node_t *temp = (node_t *)malloc(sizeof(node_t));
    temp->data = data;
    temp->next = head;
    head = temp;
    return head;
}
node_t *addLast(node_t *head, int data)
{
    if (head == NULL)
    {
        return addFirst(head,data);
    }
    node_t *iter;
    iter = head;
    while (iter->next != NULL)
    {
        iter = iter->next;
    }
    node_t *temp = (node_t *)malloc(sizeof(node_t));
    temp->data = data;
    iter->next = temp;
    temp->next = NULL;
    return head;
}
int isListEmpty(node_t *head){
    return (head == NULL);

}
node_t *findNode(node_t *head, int data){
    node_t *iter = head;
    while (iter != NULL && iter->data != data)
    {
        iter = iter->next;
    }
    return iter; 
}
void removeNode(node_t *head, int data){
    node_t *pos,*temp;
	pos = findPrevNode(head,data);
    if (pos!=NULL)
    {
        if (pos->next != NULL) {
            temp = pos->next;
            pos->next = temp->next;
            free(temp);
	    }
    }
}
node_t *findPrevNode(node_t *head, int data){
    node_t *iter = head;
    while (iter->next != NULL && iter->next->data != data)
    {
        iter = iter->next;
    }
    if ( iter->next->data == data)
    {
        return iter;
    }
    return NULL; 

}
void printList(node_t *head){
    node_t *iter = head;
    while (iter != NULL)
    {
        printf("%d\n",iter->data);
        iter = iter->next;
    }
}
int indexOf(node_t *head, int data){
    node_t *iter = head;
    int i = 0;
    while (iter != NULL && iter->data != data)
    {
        iter = iter->next;
        i++;
    }
    if (iter != NULL)
    {
        return i;
    }
    return -1;
}
void removeIndex(node_t *head,int index){
    node_t *iter = head;
    int i = 0;
    while (iter != NULL && i != index)
    {
        iter = iter->next;
        i++;
    }
    if (iter != NULL)
    {
        removeNode(head,iter->data);
    }
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
/*int main(int argc, char const *argv[])
{
    node_t *head = NULL;
    for (int i = 1; i < 6; i++)
    {
        head = addLast(head,i);
    }
    removeNode(head,3);
    removeIndex(head,indexOf(head,5));
    printList(head);
    //printf("%d\n",);
    freeList(head);
    return 0;
}*/

