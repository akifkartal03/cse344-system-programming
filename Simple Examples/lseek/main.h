#ifndef LSEEK_H
#define LSEEK_H
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include <sys/types.h>

void copyNbyte(char *iFile, char *oFile,int n);
void showUsageandExit();

#endif