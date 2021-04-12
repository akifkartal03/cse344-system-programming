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

#define row 8
#define column 8
void checkArgument(int argc);
void myPrintf(const char *str);
void myStderr(const char *str);
void showUsageAndExit();
int safeRead(int fd, void* buf, size_t size);
int safeWrite(int fd, void* buf, size_t size);
int safePRead(int fd, void *buf, size_t size,int ofset);
int safePWrite(int fd, void *buf, size_t size,int offset);
int safeOpen(const char* file, int oflag);
int safeLseek(int fd, int off, int whence);
void lockFile(int fd);
void unlockFile(int fd);
void readLockFile(int fd);
void writeEndofLine(int fd, double number, int line,char *buf);
void readFile(int fd,double x[row][column+2],double y[row][column+2],int add);
void readLine(double x[row][column],double y[row][column],double x1[],double y1[],int lineNumber);
char* readFile2(int fd);
double round1_error(int fd);
double round2_error(int fd);
void getRound1Result(int fd,char *buff,int lineNumber,int numberOfCoor,double *yi,int *xi);


#endif