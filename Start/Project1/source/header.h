#ifndef MAIN_H
#define MAIN_H
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<time.h>
#include<sys/types.h> 
#include<string.h>
#include<math.h>
#include<sys/wait.h>

void createTree(int height);
void manageLeafNode();
void manageParent(int pid1, int pid2);
#endif