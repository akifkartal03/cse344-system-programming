#ifndef ADVANCED_SEARCH_H
#define ADVANCED_SEARCH_H
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "helper.h"

void traversePathRecursively(char *targetPath, const args givenArgs);
void checkGivenArguments(const char *path,const args givenArgs);
int checkFileName(char *fileName,char *fileArgName);
int checkFileSize();
int checkFileType();
int checkFilePermission();
int checkFileLinks();
void showSearchResults(int isFound);
void drawTree(char *targetPath , char *fileName);


#endif