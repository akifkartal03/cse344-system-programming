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
#include <signal.h>
#include <sys/stat.h>
#include "lagrange.h"

void checkArgument(int argc);
void myPrintf(const char *str);
void myStderr(const char *str);
void showUsageAndExit();
int safeRead(int fd, void* buf, size_t size);
int safeWrite(int fd, void* buf, size_t size);
int safeOpen(const char* file, int oflag);
int safeLseek(int fd, int off, int whence);
void lockFile(int fd);
void unlockFile(int fd);
void readLockFile(int fd);
void writeEndofLine(int fd, double number, int line,char *buf);
char* readFile(int fd);
void readLine(int fd,char *buff,int lineNumber,int numberOfCoor,double x[],double y[],double *xi);
double round1_error(int fd);
double round2_error(int fd);
double getRound1Result(int fd,char *buff,int lineNumber,int numberOfCoor,double *yi,int count);


#endif