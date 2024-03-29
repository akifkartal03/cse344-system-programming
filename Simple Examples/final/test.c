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
#include "hash.h"
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
void readFile(int fd){
    int offset = 0;
    int bytes_read;
    int capacity = 50;
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
                pch = strtok (buffer,",");
                //printf ("%s,",pch);
                while (pch != NULL)
                {
                    printf ("%s,",pch);
                    pch = strtok (NULL, ",");
                }

                printf("\n");
                free(buffer);
                buffer = (char *)calloc(50, sizeof(char));
                capacity = 50;
                i = 0;
                offset = 0;
            }
        }

    } while (bytes_read == 1);
    free(buffer);

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
void print_list(LIST L)
{
    while (L->next != NULL) {
        for (int i = 0; i < 7; ++i) {
            printf("%d ", L->next->element->cells[i].element);
        }
        L = L->next;
    }
    printf("\n");
}
pos hashfunc(int e)
{
    return e % 10;
}

int cmp_int(int a, int b)
{
    return a == b;
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

    //int fd = safeOpen("bus.csv", O_RDONLY);
    //readFile(fd);
    int k = 0;
    int array[] = {4371, 1323, 6173, 4199, 4344, 9679, 1989};
    hash_table_ptr tbl = hash_init_table(SIZE, hashfunc, cmp_int);
    for (; k < 7; k++) {
        hash_insert(tbl, array[k]);
    }
    LIST list = list_create();
    list_insert_header(tbl, list);
    //position p = list_find(tbl, list);
    list_insert_header(tbl, list);
    list_insert_header(tbl, list);
    printf("listV1:\n");
    print_list(list);
    list_reverse(list);
    printf("listV2:\n");
    print_list(list);
    //list_delete(5, list);
    hash_destroy_table(&tbl);
    list_destroy(&list);
    return 0;
}