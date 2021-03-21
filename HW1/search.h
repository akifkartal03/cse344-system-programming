#ifndef ADVANCED_SEARCH_H
#define ADVANCED_SEARCH_H
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

void traversePathRecursively(char *targetPath);
void checkGivenArguments(const char *path);
void showSearchResults(int isFound);
void drawTree(char *targetPath , char *filneName);


#endif