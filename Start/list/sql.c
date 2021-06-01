
#include "sql.h"


char* mySelect(char *query){
    char tempQuery[strlen(query)];
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
            return selectParser(tempQuery);
        }
    }
    return NULL;
}

char *selectParser(char *query){
    char type[3] = "*";
    char *token;
    token = strtok (query," ");
    if (token != NULL) {
        token = strtok(NULL, " ");
        while (token != NULL)
        {
            if(strcmp(token,type) == 0){
                return getFullTable();
            }
            token = strtok (NULL, " ");
        }
    }

}
char* mySelectDist(char *query);
int update(char *query);
void errExit2(char *msg)
{
    //In case of an arbitrary error,
    //exit by printing to stderr a nicely formatted informative message.
    fprintf(stderr, "%s:%s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}
int safeRead2(int fd, void *buf, size_t size)
{
    int rd = read(fd, buf, size);
    if (rd == -1)
    {
        errExit2("reading error!");
    }
    return rd;
}
void readFile(int fd,int *recordSize){
    int offset = 0;
    int bytes_read;
    int capacity = 50;
    int isFirst = 1;
    int i = 0;
    char c;
    char * parsed;
    char *buffer = (char *)calloc(50, sizeof(char));
    do
    {
        bytes_read = safeRead2(fd, &c, 1);
        offset += bytes_read;
        if (capacity <= offset + 1)
        {
            capacity = capacity + 20;
            buffer = realloc(buffer, capacity * sizeof(char));
        }

        if(c != '\n' && bytes_read == 1){
            buffer[i] = c;
            i++;
        }
        else{
            /*if (!isFirst && bytes_read == 1)
                recordSize++;*/
            if(bytes_read == 1){
                buffer[i] = '\0';

                //printf ("bufer:%s\n",buffer);
                //printf("hreeeeeee\n");
                //char* tmp = strdup(buffer);
                //char temp[capacity];
                //strcpy(temp,buffer);
                //printf ("temp:%s\n",temp);
                int j = 0;
                parsed = strtok (buffer,",");
                //printf ("%s,",parsed);
                while (parsed != NULL)
                {
                    if (isFirst){
                        head = addLast(head,parsed,0,10);
                        //printf ("parsed:%s\n",parsed);
                    }
                    else{
                        //printf("iter:%s\n",iter->next->columnName);
                        //printf ("head:%s\n",head->columnName);
                        node_t *node = findByIndex(head,j);

                        if (node->capacity <= node->size + 1){
                            node->capacity = node->capacity + 10;
                            node->data = realloc(node->data, node->capacity * sizeof(char*));
                        }
                        node->data[node->size] = (char*) calloc(strlen(parsed)+1,sizeof(char));
                        strcpy(node->data[node->size],parsed);
                        node->size = node->size + 1;
                        recordSize++;
                        //printf("size:%d\n",node->size);
                        //printf("size:%d\n",node->capacity);
                    }
                    //printf ("%s,",parsed);
                    j++;
                    parsed = strtok (NULL, ",");

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
}
char *getFullTable(){
    int capacity = 50;
    char *data = (char*)calloc(50,sizeof(char));
    node_t *iter = head;
    int i = -1;
    while(iter!=NULL){
        if(i<0){
            if(strlen(data) + strlen(iter->columnName) + 3 <= capacity){
                capacity = capacity + 50;
                data = realloc(data, capacity * sizeof(char));
            }
            strcat(data,iter->columnName);
        } else{
            if(strlen(data) + strlen(iter->data[i]) + 3 <= capacity){
                capacity = capacity + 50;
                data = realloc(data, capacity * sizeof(char));
            }
            strcat(data,iter->data[i]);
        }
        strcat(data,"\t");
        iter = iter->next;
        if(iter == NULL && i < head->size){
            iter = head;
            i++;
        }
    }
    return data;
}









/*
char* get(char* query){
    char tempQuery[strlen(query)];
    strcpy(tempQuery,query);
    char type1[20] = "SELECT";
    char type2[20] = "DISTINCT";
    char type3[20] = "UPDATE";
    char *token;
    token = strtok (query," ");
    if (token != NULL){
        if(strcmp(token,type3) == 0){
            update(tempQuery);
            return NULL;
        } else{
            if(strcmp(token,type1) == 0){
                update(tempQuery);
                return NULL;
            }
            else{
                return NULL;
            }
        }

    }


}*/