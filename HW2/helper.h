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
void lockFile(int fd, struct flock *fl, int read);
void unlockFile(int fd, struct flock *fl);
void writeEndofLine(int fd, double number, int line,char *buf);
void testLagrange(char *buff,int count);


#endif