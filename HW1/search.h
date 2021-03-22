#ifndef ADVANCED_SEARCH_H
#define ADVANCED_SEARCH_H
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>
#include "helper.h"
#define BOLDRED     "\033[1m\033[31m" //to print found Files

void traversePathRecursively(args *givenArgs);
int checkGivenArguments(char *path, args *givenArgs,char *fileName);
int checkFileName(char *fileName,char *fileArgName,char *path);
int checkFileSize(struct stat fileStat,char *argSize);
int checkFileType(struct stat fileStat,char *argType);
int checkFilePermission(struct stat fileStat,char *argPermissions);
int checkFileLinks(struct stat fileStat,char *argNumber);
void showSearchResults(int isFound,char *targetPath);
void drawTree(char *targetPath , char *fileName);


#endif