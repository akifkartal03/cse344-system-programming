#ifndef HELPER_H
#define HELPER_H
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

} args;

void checkArguments(int argc, char **argv, args *givenArgs);
void my_printf(const char *str);
void my_fprintf_with_stderr(const char *str);
void showUsageAndExit();
node_t *getRegexsPositions(node_t *head, char *str, int *size);
int isRegexPos(node_t *head, int pos, char *prevChar);
int getMin(int n1, int n2, int size);


#endif
