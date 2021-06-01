
#include "sql.h"


char* mySelect(char *query){
    char tempQuery[strlen(query) + 1];
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
                return getColumns(data,0);
            }
        }
    }
    return NULL;
}
char *getColumns(char *query,int distinct){
    int c = getNumberOfColumns(query) + 1;
    int capacity = 50;
    int capacity2 = 50;
    char *data = (char*)calloc(50,sizeof(char));
    char *token;
    token = strtok (query," ,");
    node_t **nodes = (node_t **)calloc (c,sizeof(node_t *));
    char **col;
    if(distinct){
        col = (char **)calloc (c,sizeof(char *));
        for (int l = 0; l < c; ++l) {
            col[l] = (char*)calloc(50,sizeof(char));
        }
    }
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
                if(distinct){
                    if(strlen(col[k]) + strlen(nodes[k]->data[j]) + 1 <= capacity2){
                        capacity2 = capacity2 + 50;
                        col[k] = realloc(col[k], capacity2 * sizeof(char));
                    }
                }
                if(distinct){
                    if(strstr(col[k],nodes[k]->data[j]) == NULL){
                        strcat(data,nodes[k]->data[j]);
                        strcat(data,"\t");
                    }
                    else{
                        if(c > 1 && data[strlen(data) - 1] != '\t')
                            strcat(data,"\t");
                    }
                    strcat(col[k],nodes[k]->data[j]);
                }
                else{
                    strcat(data,nodes[k]->data[j]);
                    strcat(data,"\t");
                }

            }
        }
        if(data[strlen(data)-1] != '\n')
            strcat(data,"\n");
    }
    if(distinct){
        for (int t = 0; t < c; ++t) {
            free(col[t]);
        }
        free(col);
    }
    free(nodes);
    return data;
}
int getNumberOfColumns(char *str){
    int i, count;
    for (i=0, count=0; str[i]; i++)
        count += (str[i] == ',');
    return count;
}
char* mySelectDist(char *query){
    printf("DIsttt!\n");
    char *token;
    token = strtok (query," ");
    if (token != NULL) {
        token = strtok(NULL, " ");
        if (token != NULL)
        {
            token = strtok(NULL, " ");
            if(token != NULL){
                printf("token:%s\n",token);
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
                return getColumns(data,1);
            }

        }
    }
    return NULL;
}
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
                        *recordSize= *recordSize + 1;
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
int safeOpen2(const char *file, int oflag)
{
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IRWXU;
    int fd = open(file, oflag, mode);
    if (fd < 0)
    {
        if (errno == EEXIST)
        {
            remove(file);
            return safeOpen2(file, oflag);
        }
        errExit2("open error!");
    }
    return fd;
}

int main()
{

    char query[80] = "SELECT status, percent_population_change FROM TABLE";
    int fd = safeOpen2("nat.csv", O_RDONLY);
    int record;
    readFile(fd,&record);
    printf("now freee!!\n");
    //mySelect(query);
    printf("%s\n", mySelect(query));
    printf("record:%d\n",record);
    return 0;
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