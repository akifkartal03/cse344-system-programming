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

void checkArgument(int argc, char **argv);
void showUsageAndExit();
void my_printf(const char *str);
void my_stderr(const char *str);
int safeRead(int fd, void* buf, size_t size);
int safeWrite(int fd, void* buf, size_t size);
int safeOpen(const char* file, int oflag);
int safeLseek(int fd, __off_t off, int flag);
void lockFile(int fd, struct flock *fl, int read);
void unlockFile(int fd, struct flock *fl);


#endif