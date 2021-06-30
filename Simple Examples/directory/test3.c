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
    
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            
            if (dp->d_type == DT_DIR)
            {
                 // Construct new path from our base path
                strcpy(path, basePath);
                sprintf(path, "%s/%s",path, dp->d_name);
                printf("%s:::%s\n", dp->d_name,path);
                //printf("%s\n", basePath);
                listFilesRecursively(path);
            }
            else
            {
                printf("%s:::%s\n", dp->d_name,basePath);
            }
            
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