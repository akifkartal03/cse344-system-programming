
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
            char str2[len + 3];
            sprintf(str2, "%d\t", rSize);
            strcat(data,str2);
            rSize++;

        }
        freeList(distHead);
    }
    strcat(data,"\"\'");
    int len2 = (int)((ceil(log10(rSize))+1)*sizeof(char)) + 5;
    int returnSize = strlen(data) + len2;
    char *returnData = (char*)calloc(returnSize,sizeof(char));
    char str3[len2];
    sprintf(str3, "%d\n", rSize-2);
    strcat(returnData,str3);
    strcat(returnData,data);
    free(data);
    free(nodes);
    free(query);
    return returnData;
}
int getNumberOfColumns(char *str){
    int i, count;
    for (i=0, count=0; str[i]; i++)
        count += (str[i] == ',');
    return count;
}
void initCriticRows(criticRows *c){
    c->capacity=0;
    c->size = 0;
    c->data = NULL;
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
                //printf("token:%s\n",token);
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
    int i = 0;
    char *columns = strstr(query,"SET");
    if(columns!= NULL){
        char temp[strlen(columns) + 1];
        strcpy(temp,columns);
        for (int j = 0; j < 4; ++j) columns++;
        char *pos = strstr (columns,"WHERE");
        for (int j = 0; j < 6; ++j) pos++;
        char condition[strlen(pos) +1 ];
        strcpy(condition,pos);
        for (int j = 0; j < 7; ++j) pos--;
        pos[0] = '\0';

        char *col = strtok(columns, ",");
        if(col == NULL)
            colHead = addLast(colHead,columns,0,1);
        while (col != NULL){
            colHead = addLast(colHead,col,0,1);
            col = strtok(NULL,",");
        }
        char *condCol = strtok (condition,"=");
        char *condData = strtok(NULL,"=");


        if (condCol != NULL && condData != NULL){
            char *quote  = strstr(condData,"'");
            if (quote != NULL){
                quote++;
                if(quote[strlen(quote) - 1] == ';')
                    quote[strlen(quote) - 2] = '\0';
                else{
                    quote[strlen(quote) - 1] = '\0';
                }

            } else{
                quote = condData;
            }

            node_t *node = find(head,condCol);

            if(node != NULL){
                for (int j = 0; j < node->size; ++j) {
                    if(strcmp(node->data[j],quote) == 0){
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
        freeList(colHead);

        return i;
    }

    return 0;
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
    criticRows rows;
    initCriticRows(&rows);
    int doubleQuote = 0;
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
        if (capacity <= offset + 3)
        {
            capacity = capacity + 20;
            buffer = realloc(buffer, capacity * sizeof(char));
        }
        if(doubleQuote){
            if (rows.data[rows.size - 1].capacity  <= rows.data[rows.size - 1].index + 1)
            {
                rows.data[rows.size - 1].capacity  = rows.data[rows.size- 1].capacity  + 20;
                rows.data[rows.size - 1].criticRow = realloc(rows.data[rows.size - 1].criticRow, rows.data[rows.size - 1].capacity * sizeof(char));
            }
            rows.data[rows.size - 1].criticRow[rows.data[rows.size- 1].index] = c;
            rows.data[rows.size- 1].index++;
        }
        if(c == '\"'){
            if (!doubleQuote){
                doubleQuote = 1;
                if (rows.data == NULL){
                    rows.data= (criticData *)calloc(1, sizeof(criticData));
                    initCritic(&rows.data[rows.size]);
                    rows.data[rows.size].criticRow = (char *)calloc(50, sizeof(char));
                    rows.data[rows.size].capacity = 50;
                    rows.capacity = 1;
                    rows.size = rows.size + 1;

                }
                else{
                    rows.capacity  = rows.capacity  + 1;
                    rows.data = realloc(rows.data, rows.capacity * sizeof(criticData));
                    rows.size = rows.size + 1;
                }

            }
            else
                doubleQuote = 0;
        }
        //aradaki bosluk
        if(c == ',' && i > 0 && buffer[i-1] == ','){
            buffer[i] = '\t';
            i++;
        }
        if(c == ',' && i == 0 ){
            buffer[i] = '\t';
            i++;
        }
        if(c != '\n' && bytes_read == 1){
            buffer[i] = c;
            i++;
        }
        else{

            if (!isFirst && bytes_read == 1)
                *recordSize = (*recordSize) + 1;
            if(bytes_read == 1){
                if(buffer[i-1] == ','){
                    //printf("hereee!!;\n");
                    buffer[i] = '\t';
                    i++;
                }
                if(buffer[i-1] == (char)13){
                    //printf("hereee!!;\n");
                    buffer[i-1] = '\0';
                    i++;
                }
                buffer[i] = '\0';
                for (int l = 0; l < rows.size; ++l) {
                    rows.data[l].criticRow[rows.data[l].index] = '\0';
                }


                int j = 0;
                parsed = strtok (buffer,",");

                while (parsed != NULL)
                {
                    if (isFirst){
                        if(rows.size > 0 ){
                            int pi = isCritic(parsed);
                            if (pi == 1){
                                for (int k = 0; k < rows.size; ++k) {
                                    if(!rows.data[k].isAdded){
                                        head = addLast(head,rows.data[k].criticRow,0,10);
                                        rows.data[k].isAdded = 1;
                                        break;
                                    }
                                }
                            }
                            else if(pi == 0){
                                head = addLast(head,parsed,0,10);
                            }
                        }
                        else{
                            head = addLast(head,parsed,0,10);
                        }


                    }
                    else{
                        node_t *node = findByIndex(head,j);

                        if (node->capacity <= node->size + 1){
                            node->capacity = node->capacity + 10;
                            node->data = realloc(node->data, node->capacity * sizeof(char*));
                        }
                        if(rows.size > 0 ){
                            int res = isCritic(parsed);
                            if(res == 1){
                                for (int k = 0; k < rows.size; ++k) {
                                    if(!rows.data[k].isAdded){
                                        node->data[node->size] = (char*) calloc(strlen(rows.data[k].criticRow)+1,sizeof(char));
                                        strcpy(node->data[node->size],rows.data[k].criticRow);
                                        rows.data[k].isAdded = 1;
                                        node->size = node->size + 1;
                                        j++;
                                        break;
                                    }
                                }
                            }
                            else if(res == 0){
                                node->data[node->size] = (char*) calloc(strlen(parsed)+1,sizeof(char));
                                strcpy(node->data[node->size],parsed);
                                node->size = node->size + 1;
                                j++;
                            }
                        }else{


                            node->data[node->size] = (char*) calloc(strlen(parsed)+1,sizeof(char));
                            strcpy(node->data[node->size],parsed);
                            node->size = node->size + 1;
                            j++;

                        }

                    }



                    parsed = strtok (NULL, ",");

                }


                free(buffer);
                buffer = (char *)calloc(50, sizeof(char));
                capacity = 50;
                i = 0;
                offset = 0;
                doubleQuote = 0;
                destroyCritic(&rows);
                if (isFirst){
                    isFirst = 0;

                }
            }
        }

    } while (bytes_read == 1);
    free(buffer);
}
void initCritic(criticData *c){
    c->isAdded = 0;
    c->criticRow = NULL;
    c->capacity = 0;
    c->index = 0;
}
void destroyCritic(criticRows *c){
    if(c->size > 0){
        for (int i = 0; i < c->size; ++i) {
            free(c->data[i].criticRow);
        }
        free(c->data);
        c->data = NULL;
        c->size = 0;
        c->capacity = 0;
    }

}
int isContainComma(criticRows c){
    if(c.size == 0)
        return 0;
    else{
        for (int i = 0; i < c.size; ++i) {
            char *pos;
            if ((pos=strchr(c.data[i].criticRow, ',')) != NULL)
                return 1;
        }
    }
    return 0;
}
int isCritic(char *data){
    char *pos;
    if ((pos=strchr(data, '\"')) != NULL){
        if(data[strlen(data)-1] != '\"')
            return 1;
        else{
            return -1;
        }
    }
    return 0;
}
void myStrChar(char *str,char c){

    size_t cur_len = strlen(str);
    str[cur_len] = c;
    str[cur_len+1] = '\0';
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
        else{
            strcat(data,"\t");
        }
    }
    strcat(data,"\"\'");
    int len = (int)((ceil(log10(i+1))+1)*sizeof(char)) + 5;
    int returnSize = strlen(data) + len;
    char *returnData = (char*)calloc(returnSize,sizeof(char));
    char str[len];
    sprintf(str, "%d\n", i);
    strcat(returnData,str);
    strcat(returnData,data);
    free(data);
    return returnData;
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

    int j = 0;
    int size  = strlen(result);
    for (j = 0; j < size; j++) {
        if (result[j] == '\n'){
            break;
        }
    }

    char number[j + 2];
    for (int k = 0; k < j + 2; ++k) {
        if (result[k] != '\n'){
            number[k] = result[k];
        }
        else{
            number[k] = '\0';
            break;
        }
    }
    return atoi(number);
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