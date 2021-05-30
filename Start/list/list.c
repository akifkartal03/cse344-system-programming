
#include "list.h"

node_t *addFirst(node_t *head, char *cName, char **cData,int s)
{
    node_t *temp = (node_t *)malloc(sizeof(node_t));
    temp->data = cData;
    temp->columnName = cName;
    temp->size = s;
    temp->next = head;
    head = temp;

    return head;
}
node_t *addLast(node_t *head, char *cName, char **cData,int s)
{
    if (head == NULL)
    {
        return addFirst(head, cName, cData,s);
    }
    node_t *iter;
    iter = head;
    while (iter->next != NULL)
    {
        iter = iter->next;
    }
    node_t *temp = (node_t *)malloc(sizeof(node_t));
    temp->data = cData;
    temp->columnName = cName;
    temp->size = s;
    iter->next = temp;
    temp->next = NULL;

    return head;
}
void freeList(node_t *head)
{
    node_t *iter;
    while (head != NULL)
    {
        iter = head;
        head = head->next;
        /*for (int i = 0; i < iter->size; ++i) {
            free(iter->data[i]);
        }
        printf("after forr\n");
        free(iter->columnName);
        printf("after columnn\n");*/
        free(iter);
    }
}
void printList(node_t *head){
    node_t *iter = head;
    while (iter != NULL)
    {
        printf("ColumnName:%s\n",iter->columnName);
        for (int i = 0; i < iter->size; ++i) {
            printf("%s, ",iter->data[i]);
        }
        printf("\n");
        iter = iter->next;
    }
}