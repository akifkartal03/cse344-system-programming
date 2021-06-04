
#include "list.h"

node_t *addFirst(node_t *head, char *cName, int s,int c)
{
    node_t *temp = (node_t *)malloc(sizeof(node_t));
    //temp->data = cData;
    temp->data = (char**)calloc (c,sizeof(char*));
    temp->columnName = (char*) calloc(strlen(cName)+1,sizeof(char));
    strcpy(temp->columnName,cName);
    //free(cName);
    //temp->columnName = cName;
    temp->size = s;
    temp->capacity = c;
    temp->next = NULL;
    head = temp;

    return head;
}
node_t *addLast(node_t *head, char *cName, int s,int c)
{
    if (head == NULL)
    {
        return addFirst(head, cName,s,c);
    }
    node_t *iter;
    iter = head;
    while (iter->next != NULL)
    {
        iter = iter->next;
    }
    node_t *temp = (node_t *)malloc(sizeof(node_t));
    temp->data = (char**)calloc (c,sizeof(char*));
    //temp->data = cData;
    temp->columnName = (char*) calloc(strlen(cName)+1,sizeof(char));
    strcpy(temp->columnName,cName);
    //free(cName);
    //temp->columnName = cName;
    temp->size = s;
    temp->capacity = c;
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
        for (int i = 0; i < iter->capacity; ++i) {
            free(iter->data[i]);
        }
        free(iter->columnName);
        free(iter->data);
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
node_t *find(node_t *head, char *cName){
    node_t *iter = head;
    while (iter != NULL)
    {
        if(strcmp(cName,iter->columnName) == 0)
            return iter;
        iter = iter->next;
    }
    return NULL;
}
node_t *findByIndex(node_t *head, int index){
    node_t *iter = head;
    int i = 0;
    if(iter->size<0){
        printf("heree\n");
        printf("%s",iter->columnName);
    }

    while (iter != NULL)
    {
        if(index == i)
            return iter;
        iter = iter->next;
        i++;
    }
    return NULL;
}
void set(node_t *head, char *cName, int index, char *newData){
    node_t *node = find(head,cName);
    if (node != NULL){
        free(node->data[index]);
        node->data[index] = (char*) calloc(strlen(newData)+1,sizeof(char));
        strcpy(node->data[index],newData);
    }
}