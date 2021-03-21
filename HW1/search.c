#include "search.h"

void traversePathRecursively(char *targetPath, const args givenArgs){
    char currentPath[PATH_MAX];
    DIR *dir;
    struct dirent *entry;
    size_t pathLength = strlen(targetPath);
    if (pathLength >= PATH_MAX)
    {
        fprintf(stderr, "Path length is longer than expected!\n");
        exit(EXIT_FAILURE);
    }
    dir = opendir(targetPath);
    if (!dir)
    {
       return;
    }
    strcpy(currentPath, targetPath);
    strcat(currentPath, "/");
    while ((entry = readdir(dir)) != NULL)
    {
       //check directory is different than current and parent
       if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            //checkGivenArguments(entry->d_name, givenArgs);
            printf("%s\n",currentPath);
            strcpy(currentPath, targetPath);
            strcat(currentPath, "/");
            strcat(currentPath, entry->d_name);
            size_t pathLength = strlen(currentPath);
            if (pathLength >= PATH_MAX)
            {
                fprintf(stderr, "Path length is longer than expected!\n");
                exit(EXIT_FAILURE);
            }
            traversePathRecursively(currentPath,givenArgs);
        }
    }
    if (close(dir))
    {
        fprintf(stderr, "Directory close error! %s\n",strerror(errno));
        exit(EXIT_FAILURE);
    }
    
}
void checkGivenArguments(const char *path,const args givenArgs){
    if (givenArgs.fFlag)
    {
        checkFileName(path,givenArgs.fArg);
    }
    
}
int checkFileName(char *fileName,char *fileArgName){
    return 0;
}
int checkFileSize();
int checkFileType();
int checkFilePermission();
int checkFileLinks();
void showSearchResults(int isFound);
void drawTree(char *targetPath , char *fileName);

int main(int argc, char *argv[])
{
    args a;
    traversePathRecursively(argv[1],a);
    //printf("count: %d\n",a.count);
    //printf("filename main: %s\n",a.fArg);
    return 0;
}