#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include<signal.h>
#include<unistd.h>
#include <stdlib.h>

void listFilesRecursively(char *path);
void sig_handler(int signo)
{
  if (signo == SIGINT){
      printf("EXITING...\n");
      exit(0);
  }
    
}

int main()
{

    signal(SIGINT, sig_handler);
    // Directory path to list files
    char path[100];

    // Input path from user
    printf("Enter path to list files: ");
    scanf("%s", path);

    listFilesRecursively(path);

    return 0;
}


/**
 * Lists all files and sub-directories recursively 
 * considering path as base path.
 */
void listFilesRecursively(char *basePath)
{
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    // Unable to open directory stream
    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            printf("%s\n", dp->d_name);

            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            listFilesRecursively(path);
        }
    }

    closedir(dir);
}