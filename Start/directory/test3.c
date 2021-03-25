#include "signal.h"

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
        if (exitSignal)
        {
            printf("EXITING....\n");
            exit(0);
        }
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            printf("%s\n", dp->d_name);

            // Construct new path from our base path
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            if (exitSignal)
            {
                printf("EXITING....\n");
                exit(0);
            }
            listFilesRecursively(path);
        }
    }

    closedir(dir);
}
void sig_handler(int signo)
{
  if (signo == SIGINT){
      exitSignal=1;
  }
    
}