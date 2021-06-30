#ifndef SIGNAL_H
#define SIGNAL_H
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include<signal.h>
#include<unistd.h>
#include <stdlib.h>



void listFilesRecursively(char *path);
void sig_handler(int signo);

#endif