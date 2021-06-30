

#include "sql_engine.h"
Table table;
void initTable(){
    table.size = 0;
    table.numberOfColumn = 5;
    table.capacity = 50;
    table.data = (char ***)calloc(table.capacity, sizeof(char**));
    table.data[table.size] = (char **)calloc(table.numberOfColumn, sizeof(char*));
}

void readFile(int fd,int *recordSize){
    criticRows rows;
    initCriticRows(&rows);
    initTable();
    printf("heree\n");
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
                    buffer[i] = '\t';
                    i++;

                }
                /*buffer[i] = '\n';
                i++;*/
                buffer[i] = '\0';
                for (int l = 0; l < rows.size; ++l) {
                    rows.data[l].criticRow[rows.data[l].index] = '\0';
                }

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
                    if(rows.size > 0 ){
                        int pi = isCritic(parsed);
                        if (pi == 1){
                            for (int k = 0; k < rows.size; ++k) {
                                if(!rows.data[k].isAdded){
                                    table.data[table.size][j] =  (char *)calloc(strlen(rows.data[k].criticRow) +3, sizeof(char));
                                    strcpy(table.data[table.size][j],rows.data[k].criticRow);
                                    //strcat(table.data[table.size][j],"\t");
                                    rows.data[k].isAdded = 1;
                                    break;
                                }
                            }
                        }
                        else if(pi == 0){
                            table.data[table.size][j] =  (char *)calloc(strlen(parsed) + 3, sizeof(char));
                            strcpy(table.data[table.size][j],parsed);
                            //strcat(table.data[table.size][j],"\t");
                        }
                    }
                    else{
                        table.data[table.size][j] =  (char *)calloc(strlen(parsed) + 3, sizeof(char));
                        strcpy(table.data[table.size][j],parsed);
                        //strcat(table.data[table.size][j],"\t");
                    }
                    j++;
                    if (isFirst && j >= table.numberOfColumn){

                        table.numberOfColumn = table.numberOfColumn + 5;
                        table.data[table.size] = realloc(table.data[table.size], table.numberOfColumn * sizeof(char*));
                    }
                    parsed = strtok (NULL, ",");

                }

                //printf("\n");
                free(buffer);
                buffer = (char *)calloc(50, sizeof(char));
                capacity = 50;
                i = 0;
                offset = 0;
                doubleQuote = 0;
                destroyCritic(&rows);
                if (isFirst){
                    isFirst = 0;
                    table.numberOfColumn = j;
                }
                table.size = table.size + 1;
                if (table.size + 1 <= table.capacity){
                    table.capacity = table.capacity + 50;
                    table.data = realloc(table.data, table.capacity * sizeof(char**));
                }
                table.data[table.size] = (char **)calloc(table.numberOfColumn, sizeof(char*));

            }
        }

    } while (bytes_read == 1);
    free(buffer);
}
char *getFullTable(){
    printf("size:%d\n",table.size);
    printf("col:%d\n",table.numberOfColumn);
    int capacity = 50;
    char *result = (char *)calloc(capacity, sizeof(char));
    strcat(result,"\t");
    int i;
    for (i = 0; i < table.size; ++i) {
        for (int j = 0; j < table.numberOfColumn; ++j) {
            if(strlen(table.data[i][j]) + strlen(result) + 10 >= capacity){
                capacity = capacity + 50 + strlen(table.data[i][j]);
                result = realloc(result, capacity * sizeof(char));
            }
            if(i != 0 && j == 0){
                int len = (int)((ceil(log10(i))+1)*sizeof(char));
                //printf("heree1:%d\n",len);
                char str[len + 3];
                sprintf(str, "%d\t", i);
                strcat(result,str);
            }
            strcat(result,table.data[i][j]);
            strcat(result,"\t");
        }
        strcat(result,"\n");
    }
    int len = (int)((ceil(log10(i))+1)*sizeof(char));
    //printf("heree1:%d\n",len);
    char str[len + 3];
    sprintf(str, "%d\t\n", i);
    strcat(result,str);
    return result;
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
void errExit2(char *msg)
{
    //In case of an arbitrary error,
    //exit by printing to stderr a nicely formatted informative message.
    fprintf(stderr, "%s:%s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}
void initCriticRows(criticRows *c){
    c->capacity=0;
    c->size = 0;
    c->data = NULL;
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
void errExit(char *msg)
{
    //In case of an arbitrary error,
    //exit by printing to stderr a nicely formatted informative message.
    fprintf(stderr, "%s:%s\n", msg, strerror(errno));
    exit(EXIT_FAILURE);
}
void printTable(){
    for (int i = 0; i < table.size; ++i) {
        for (int j = 0; j < table.numberOfColumn; ++j) {
            printf("%s",table.data[i][j]);
        }
    }
    printf("\n");
}
void freeTable(){
    for (int i = 0; i < table.size; ++i) {
        for (int j = 0; j < table.numberOfColumn; ++j) {
           free(table.data[i][j]);
        }
        free(table.data[i]);
    }
    free(table.data);
}
int main()
{
    //char query[50] = "SELECT * FROM TABLE";
    //char query2[75] = "UPDATE TABLE SET natural_increase = 5000 WHERE status = 'P'";
    //char query3[60] = "SELECT period FROM TABLE";
    //char query4[75] = "SELECT DISTINCT percent_population_change, status FROM TABLE";
    /*
    char test[100]  = "Alfonsino & Long-finned Beryx,1996,Asset value,Dollars,Millions,,Environmental Accounts,20.3";
    char *parsed;
    int j = 0;
    parsed = strtok (test,",");
    //printf ("%s,",parsed);
    while (parsed != NULL)
    {
        printf ("%s\n",parsed);
        printf("%lu\n", strlen(parsed));
        j++;
        parsed = strtok (NULL, ",");

    }
    printf("%d\n",j);
    */

    int recor= 0;
    int fd = safeOpen2("Zealand.csv",O_RDONLY);
    readFile(fd,&recor);
    printf("File loaded!\n");
    char *result = getFullTable();
    printf("%s",result);
    //printTable();



    free(result);
    freeTable();

    return 0;
}