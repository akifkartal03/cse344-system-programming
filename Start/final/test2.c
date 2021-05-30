#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "list.h"

void print_list(LIST L)
{
    while (L->next != NULL) {
        for (int i = 0; i < 3; ++i) {
            printf("%s ", L->next->element[i]);
        }
        L = L->next;
    }
    printf("\n");
}

int main()
{
    int rows = 3;
    char **c = calloc (rows,sizeof(char*));
    c[0] = "cat";
    c[1] = "dog";
    c[2] = "mouse";
    char **c1 = calloc (rows,sizeof(char*));
    c1[0] = "cat1";
    c1[1] = "dog1";
    c1[2] = "mouse1";
    char **c2 = calloc (rows,sizeof(char*));
    c2[0] = "cat2";
    c2[1] = "dog2";
    c2[2] = "mouse2";
    LIST list = list_create();
    assert(list_is_empty(list));
    list_insert_header(c, list);
    assert(!list_is_empty(list));
    position p = list_find(c, list);
    assert(list_is_last(p, list));
    list_insert_header(c1, list);
    list_insert_header(c2, list);
    printf("listV1:\n");
    print_list(list);
    list_reverse(list);
    printf("listV2:\n");
    print_list(list);
    //list_delete(5, list);
    //assert(list_find(5, list) == NULL);
    return 0;
}