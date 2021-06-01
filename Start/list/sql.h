
#ifndef _SQL_H
#define _SQL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include "list.h"

node_t *head = NULL;

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


#endif
