#ifndef _SQL_ENGINE_H_
#define _SQL_ENGINE_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include "list.h"

typedef struct readData{
    int isAdded;
    char *criticRow;
    int capacity;
    int index;
}criticData;
typedef struct critics{
    criticData *data;
    int capacity;
    int size;

}criticRows;

extern node_t *head;

void readFile(int fd,int *recordSize);
char *getFullTable();
char* mySelect(char *query);
char* mySelectDist(char *query);
char *selectParser(char *query);
int update(char *query);
int safeRead2(int fd, void *buf, size_t size);
int safeOpen2(const char *file, int oflag);
void errExit2(char *msg);
char *getColumns(char *query,int distinct);
int getNumberOfColumns(char *str);
void setColumnData(char *data,int index);
int getReturnSize(char *result);
void printData(char *result);
int getQueryTypeEngine(char *query);
void initCritic(criticData *c);
void initCriticRows(criticRows *c);
int isContainComma(criticRows c);
int isCritic(char *data);
void destroyCritic(criticRows *c);
void myStrChar(char *str,char c);

#endif
