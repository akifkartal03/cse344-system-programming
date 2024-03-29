#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <pthread.h>
#include <float.h>
#include <limits.h>
#include "list.h"

#define SIZE 10
#define SIZE2 7
void getfield(char* line)
{
    /*char *removed = strchr(line, '\n');
    if(removed!=NULL)
        line[strlen(line)-2] = '\0';*/

    printf ("line1:%c,",line[strlen(line)-1]);
    printf ("line1:%c,",line[strlen(line)-2]);
    char * pch;
    pch = strtok (line,",");
    while (pch != NULL)
    {
        /*char *removed = strchr(pch, '\n');
        if(removed!=NULL)
            pch[strlen(pch)-1] = '.';
        char *test = (char*) malloc(strlen(pch)+1);
        strcpy(test,pch);
        char *removed = strchr(test, '\n');
        if(removed!=NULL)
            test[strlen(test)-1] = '\0';*/
        printf ("%s,",pch);
        pch = strtok (NULL, ",");
    }
    printf ("\n");
}
void errExit(char *msg)
{
    //In case of an arbitrary error,
    //exit by printing to stderr a nicely formatted informative message.
    fprintf(stderr, "%s:%s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}
int safeRead(int fd, void *buf, size_t size)
{
    int rd = read(fd, buf, size);
    if (rd == -1)
    {
        errExit("reading error!");
    }
    return rd;
}
node_t *readFile(int fd){
    node_t *head = NULL;

    int offset = 0;
    int bytes_read;
    int capacity = 50;
    int isFirst = 1;
    int i = 0;
    char c;
    char * pch;
    char *buffer = (char *)calloc(50, sizeof(char));
    do
    {
        bytes_read = safeRead(fd, &c, 1);
        offset += bytes_read;
        if (capacity <= offset + 1)
        {
            capacity = capacity + 20;
            buffer = realloc(buffer, capacity * sizeof(char));
        }
        if (c == '\t')
            printf("hereeeee\n");
        if(c != '\n' && bytes_read == 1){
            buffer[i] = c;
            i++;
        }

        else{
            if(bytes_read == 1){
                buffer[i] = '\0';

                //printf ("bufer:%s\n",buffer);
                //printf("hreeeeeee\n");
                //char* tmp = strdup(buffer);
                //char temp[capacity];
                //strcpy(temp,buffer);
                //printf ("temp:%s\n",temp);
                int j = 0;
                pch = strtok (buffer,",");
                //printf ("%s,",pch);
                while (pch != NULL)
                {
                    if (isFirst){
                        head = addLast(head,pch,0,10);
                        //printf ("pch:%s\n",pch);
                    }
                    else{
                        //printf("iter:%s\n",iter->next->columnName);
                        //printf ("head:%s\n",head->columnName);
                        node_t *node = findByIndex(head,j);

                        if (node->capacity <= node->size + 1){
                            node->capacity = node->capacity + 10;
                            node->data = realloc(node->data, node->capacity * sizeof(char*));
                        }
                        node->data[node->size] = (char*) calloc(strlen(pch)+1,sizeof(char));
                        strcpy(node->data[node->size],pch);
                        node->size = node->size + 1;
                        //printf("size:%d\n",node->size);
                        //printf("size:%d\n",node->capacity);
                    }
                    //printf ("%s,",pch);
                    j++;
                    pch = strtok (NULL, ",");

                }

                //printf("\n");
                free(buffer);
                buffer = (char *)calloc(50, sizeof(char));
                capacity = 50;
                i = 0;
                offset = 0;
                if (isFirst){
                    isFirst = 0;
                    //printList(head);
                }
            }
        }

    } while (bytes_read == 1);
    free(buffer);
    return head;

}
char *getFullTable(node_t *head){
    int capacity = 50;
    char *data = (char*)calloc(50,sizeof(char));
    node_t *iter = head;
    int i = -1;
    int size = head->size;
    while(iter!=NULL && i < size){
        if(i<0){
            if(strlen(data) + strlen(iter->columnName) + 5 <= capacity){
                capacity = capacity + 50;
                data = realloc(data, capacity * sizeof(char));
            }
            strcat(data,iter->columnName);
        } else{
            if(strlen(data) + strlen(iter->data[i]) + 5 <= capacity){
                capacity = capacity + 50;
                data = realloc(data, capacity * sizeof(char));
            }
            strcat(data,iter->data[i]);
        }
        strcat(data,"\t");
        iter = iter->next;
        if(iter == NULL){
            iter = head;
            i++;
            strcat(data,"\n");
        }
    }
    return data;
}
int safeOpen(const char *file, int oflag)
{
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IRWXU;
    int fd = open(file, oflag, mode);
    if (fd < 0)
    {
        if (errno == EEXIST)
        {
            remove(file);
            return safeOpen(file, oflag);
        }
        errExit("open error!");
    }
    return fd;
}

int getNumberOfColumns(char *str){
    int i, count;
    for (i=0, count=0; str[i]; i++)
        count += (str[i] == ',');
    return count;
}
char *getColumns(char *query,node_t *head){
    int c = getNumberOfColumns(query) + 1;
    int capacity = 50;
    char *data = (char*)calloc(50,sizeof(char));
    char *token;
    token = strtok (query," ,");
    node_t **nodes = (node_t **)calloc (c,sizeof(node_t *));
    int i = 0;
    while (token != NULL)
    {
        nodes[i] = find(head,token);
        if(strlen(data) + strlen(token) + 5 <= capacity){
            capacity = capacity + 50;
            data = realloc(data, capacity * sizeof(char));
        }
        strcat(data,token);
        strcat(data,"\t");
        i++;
        token = strtok (NULL, " ,");
    }
    strcat(data,"\n");
    int size = head->size;
    for (int j = 0; j < size; ++j) {
        for (int k = 0; k < c; ++k) {
            if(nodes[k] != NULL){
                if(strlen(data) + strlen(nodes[k]->data[j]) + 5 <= capacity){
                    capacity = capacity + 50;
                    data = realloc(data, capacity * sizeof(char));
                }
                strcat(data,nodes[k]->data[j]);
                strcat(data,"\t");
            }

        }
        strcat(data,"\n");
    }
    free(nodes);
    return data;
}

char* mySelectDist(char *query){
    return NULL;
}
char *selectParser(char *query,node_t *head){
    char type[3] = "*";
    char *token;
    printf("quer:%s\n",query);
    token = strtok (query," ");
    if (token != NULL) {
        token = strtok(NULL, " ");
        printf("token1:%s\n",token);
        if (token != NULL)
        {
            if(strcmp(token,type) == 0){
                return getFullTable(head);
            }
            else{
                char from[6] ="FROM";
                int capacity = 50;
                char *data = (char*)calloc(50,sizeof(char));
                do{
                    if(strlen(data) + strlen(token) + 3 <= capacity){
                        capacity = capacity + 30;
                        data = realloc(data, capacity * sizeof(char));
                    }
                    strcat(data,token);
                    token = strtok(NULL, " ");
                    if (token == NULL)
                        break;
                }while(strcmp(from,token) != 0);
                if(data[strlen(data) - 1] == ',')
                    data[strlen(data) - 1] = '\0';
                return getColumns(data,head);
            }
        }
    }
    return NULL;
}
char* mySelect(char *query,node_t *head){
    char tempQuery[strlen(query)+1];
    strcpy(tempQuery,query);
    char type[10] = "DISTINCT";
    char *token;
    token = strtok (query," ");
    if (token != NULL){
        token = strtok (NULL, " ");
        if(strcmp(token,type) == 0){
            return mySelectDist(tempQuery);
        }
        else{
            return selectParser(tempQuery,head);
        }
    }
    return NULL;
}
int main()
{
    /*FILE* stream = fopen("bus.csv", "r");
    char * pch;
    char line[1024];
    while (fgets(line, 1024, stream))
    {
        char* tmp = strdup(line);
        printf ("line1:%c, ",line[strlen(tmp)-1]);
        printf ("line2:%c\n",line[strlen(tmp)-2]);
        //getfield(tmp);
        // NOTE strtok clobbers tmp
        free(tmp);
    }*/

    char query[60] = "SELECT status, period FROM TABLE";
    int fd = safeOpen("nat.csv", O_RDONLY);
    node_t *head = readFile(fd);
    printf("now freee!!\n");
    printf("%s\n", mySelect(query,head));
    //printList(head);
    //char *test = (char *) head->data;
    //printf("%s",test);
    //freeList(head);

    //printf("i:%d\n", getNumberOfColumns(query));
    /*char *token;
    token = strtok (query," ,");
    int i = 0;
    while (token != NULL)
    {
        i++;
        printf("%s,",token);
        token = strtok (NULL, " ,");
    }
    printf("\ni:%d\n",i);*/
    return 0;
}
/*int main(){
    node_t *head = NULL;
    int rows = 3;
    char *cName = "test";
    char *cName1 = "test2";
    char *cName2 = "test3";
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
    head = addLast(head,cName,c,rows);
    head = addLast(head,cName1,c1,rows);
    head = addLast(head,cName2,c2,rows);
    printList(head);
    printf("now freee!!\n");
    free(c);
    free(c1);
    free(c2);
    printf("afterr!\n");
    //free(cName);
    //free(cName1);
    //free(cName2);
    freeList(head);
    return 0;

}*/