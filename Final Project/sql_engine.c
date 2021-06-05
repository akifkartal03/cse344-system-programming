
#include "sql_engine.h"

node_t *head = NULL;
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
                unsigned int capacity = 50;
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
    printf("%d\n",distinct);
    int c = getNumberOfColumns(query) + 1;
    unsigned int capacity = 50;
    char *data = (char*)calloc(50,sizeof(char));
    char *token;
    token = strtok (query," ,");
    node_t **nodes = (node_t **)calloc (c,sizeof(node_t *));
    node_t  *distHead = NULL;
    strcat(data,"\t");
    int i = 0;
    while (token != NULL)
    {
        nodes[i] = find(head,token);
        if(strlen(data) + strlen(token) + 10 <= capacity){
            capacity = capacity + 50;
            data = realloc(data, capacity * sizeof(char));
        }
        strcat(data,token);
        strcat(data,"\t");
        i++;
        if(distinct)
            distHead = addLast(distHead,token,0,10);
        token = strtok (NULL, " ,");
    }
    strcat(data,"\n");
    int rSize = 1;
    //printf("heree1\n");
    int len = (int)((ceil(log10(rSize))+1)*sizeof(char));
    char str[len + 3];
    sprintf(str, "%d\t", rSize);
    strcat(data,str);
    int size = head->size;
    for (int j = 0; j < size; ++j) {
        for (int k = 0; k < c; ++k) {
            if(nodes[k] != NULL){
                if(strlen(data) + strlen(nodes[k]->data[j]) + 10 <= capacity){
                    capacity = capacity + 50;
                    data = realloc(data, capacity * sizeof(char));
                }
                if(distinct){
                    node_t *node = findByIndex(distHead,k);
                    if(!isContain(node,nodes[k]->data[j])){
                        if (node->capacity <= node->size + 1){
                            node->capacity = node->capacity + 10;
                            node->data = realloc(node->data, node->capacity * sizeof(char*));
                        }
                        node->data[node->size] = (char*) calloc(strlen(nodes[k]->data[j])+1,sizeof(char));
                        strcpy(node->data[node->size],nodes[k]->data[j]);
                        node->size = node->size + 1;
                    }

                }
                else{
                    strcat(data,nodes[k]->data[j]);
                    strcat(data,"\t");
                }

            }
        }
        if(!distinct){
            if(data[strlen(data)-1] != '\n'){
                strcat(data,"\n");
                rSize++;
                len = (int)((ceil(log10(rSize))+1)*sizeof(char));
                char str2[len + 3];
                sprintf(str2, "%d\t", rSize);
                strcat(data,str2);
            }
        }


    }

    if(distinct){
        int finished = 0;
        rSize++;
        for (int j = 0; j < size; ++j) {
            finished = 0;
            for (int k = 0; k < c; ++k) {
                node_t *node = findByIndex(distHead,k);
                if(node->size>j){
                    if(strlen(data) + strlen(node->data[j]) + 5 <= capacity){
                        capacity = capacity + 50;
                        data = realloc(data, capacity * sizeof(char));
                    }
                    strcat(data,node->data[j]);
                    strcat(data,"\t");
                }
                else{
                    finished++;
                }
            }
            if(finished == c)
                break;
            strcat(data,"\n");
            len = (int)((ceil(log10(rSize))+1)*sizeof(char));
            //printf("heree1:%d\n",rSize);
            char str2[len + 3];
            sprintf(str2, "%d\t", rSize);
            strcat(data,str2);
            rSize++;

        }
        freeList(distHead);
    }
    free(nodes);
    free(query);
    return data;
}
int getNumberOfColumns(char *str){
    int i, count;
    for (i=0, count=0; str[i]; i++)
        count += (str[i] == ',');
    return count;
}
char* mySelectDist(char *query){
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
                unsigned int capacity = 50;
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
int update(char *query){
    node_t *colHead = NULL;
    char *token ;
    token = strtok (query," ");
    int i = 0;
    //pass update and Table data
    while(token != NULL && i < 2){
        token = strtok (NULL," ");
        i++;
    }
    i = 0;
    char *columns = strtok (NULL,"WHERE");
    char *condition =strtok (NULL,"WHERE");
    if (columns != NULL && condition != NULL){
        char *col = strtok(columns, ",");
        while (col != NULL){
            colHead = addLast(colHead,col,0,1);
            col = strtok(NULL,",");
        }
        char *condCol = strtok (condition," =");
        char *condData = strtok(NULL," =");
        if (condCol != NULL && condData != NULL){
            char *pos  = strstr(condData,"'");
            if (pos != NULL){
                pos++;
                pos[strlen(pos) - 1] = '\0';

            } else{
                pos = condData;
            }

            node_t *node = find(head,condCol);
            if(node != NULL){
                for (int j = 0; j < node->size; ++j) {
                    if(strcmp(node->data[j],pos) == 0){
                        node_t *iter = colHead;
                        while (iter != NULL){
                            setColumnData(iter->columnName,j);
                            iter = iter->next;
                        }
                        i++;
                    }
                }
            }
        }
    }
    freeList(colHead);
    return i;
}

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
    unsigned int offset = 0;
    int bytes_read;
    unsigned int capacity = 50;
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
            if (!isFirst && bytes_read == 1)
                *recordSize = (*recordSize) + 1;
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
                        //*recordSize= *recordSize + 1;
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
    unsigned int capacity = 50;
    char *data = (char*)calloc(50,sizeof(char));
    node_t *iter = head;
    int i = -1;
    int size = head->size;
    strcat(data,"\t");
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
            int len = (int)((ceil(log10(i+1))+1)*sizeof(char));
            //printf("heree1:%d\n",len);
            char str[len + 3];
            sprintf(str, "%d\t", i+1);
            strcat(data,str);
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

void setColumnData(char *data,int index){
    // char
    char tempData[strlen(data) + 1];
    strcpy(tempData,data);
    char *colName = strtok (tempData," =");
    char *value = strtok (NULL," =");

    if (colName != NULL && value != NULL){
        char *pos  = strstr(value,"'");
        if (pos != NULL){
            pos++;
            pos[strlen(pos) - 1] = '\0';

        } else{
            pos = value;
        }
        set(head,colName,index,pos);
    }
}
int getReturnSize(char *result){
    int i = strlen(result) - 1;
    int j = 0;
    for (j = i; j >=0 ; j--) {
        if (result[j] == '\n'){
            break;
        }
    }
    int size = strlen(result) - j;
    char number[size];
    j++;
    for (int k = 0; k < size; ++k) {
        if (result[j] != '\t'){
            number[k] = result[j];
            j++;
        }
        else{
            number[k] = '\0';
            break;
        }

    }
    return atoi(number) - 1;
}
void printData(char *result){
    int i = strlen(result) - 1;
    int j = 0;
    for (j = i; j >=0 ; j--) {
        if (result[j] == '\n'){
            break;
        }
    }
    j++;
    result[j] = '\0';
    printf("%s",result);
}
int getQueryTypeEngine(char *query){
    char tempQuery[strlen(query) + 1];
    strcpy(tempQuery,query);
    char *firstToken = strtok(tempQuery," ");
    if(firstToken != NULL){
        if(strcmp(firstToken,"SELECT") == 0)
            return 1;
        else if(strcmp(firstToken,"UPDATE") == 0){
            return 2;
        }

    }
    return -1;
}
/*void test(char* a, char* b){
    if(a != NULL)
        printf("after2:%s\n",a);
    if(b != NULL)
        printf("after3:%s\n",b);
}
int main()
{

    return 0;
}*/



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
