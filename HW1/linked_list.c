#include <string.h>
#include "linked_list.h"

node_t *addFirst(node_t *head, char prevChar, int position)
{
    node_t *temp = (node_t *)malloc(sizeof(node_t));
    temp->position = position;
    temp->preChr = prevChar;
    temp->next = head;
    head = temp;
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
    return head;
}