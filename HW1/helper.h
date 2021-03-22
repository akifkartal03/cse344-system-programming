#ifndef HELPER_H
#define HELPER_H

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
#include "linked_list.h"
typedef struct st
{
    int wFlag;
    int fFlag;
    int bFlag;
    int tFlag;
    int pFlag;
    int lFlag;
    char *wArg;
    char *fArg;
    char *bArg;
    char *tArg;
    char *pArg;
    char *lArg;
    int isFound;
    int count;
    
}args;
void checkArguments(int argc, char **argv,args *givenArgs);
void showUsageAndExit();
node_t* getRegexsPositions(node_t *head,char *str,int *size);
int isRegexPos(node_t *head, int pos, char *prevChar);
int getMin(int n1,int n2, int size);

#endif
