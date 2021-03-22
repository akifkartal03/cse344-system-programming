#include "search.h"
#include "helper.c"

void traversePathRecursively(char *targetPath, const args givenArgs)
{
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
    while ((entry = readdir(dir)) != NULL)
    {
        //check directory is different than current and parent
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {

            strcpy(currentPath, targetPath);
            strcat(currentPath, "/");
            strcat(currentPath, entry->d_name);
            checkGivenArguments(currentPath, givenArgs, entry->d_name);
            //printf("%s\n",currentPath);
            size_t pathLength = strlen(currentPath);
            if (pathLength >= PATH_MAX)
            {
                fprintf(stderr, "Path length is longer than expected!\n");
                exit(EXIT_FAILURE);
            }
            traversePathRecursively(currentPath, givenArgs);
        }
    }
    if (closedir(dir))
    {
        fprintf(stderr, "Directory close error! %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}
void checkGivenArguments(char *path, const args givenArgs, char *fileName)
{
    int options = 0;
    if (givenArgs.fFlag)
    {
        if (checkFileName(fileName, givenArgs.fArg, path))
        {
            options++;
        }
    }
    if (givenArgs.bFlag)
    {
        if (checkFileSize(path,givenArgs.bArg))
        {
            options++;
        }
        
    }
    
}
int checkFileName(char *fileName, char *fileArgName, char *path)
{
    node_t *head = NULL;
    int size = 0;
    //printf("burdaaaa\n");
    //printf("arg name: %s\n",fileArgName);
    //printf("filename: %s\n",fileName);
    head = getRegexsPositions(head, fileArgName, &size);
    char prevChar, c1, c2;
    int len1 = strlen(fileArgName);
    int len2 = strlen(fileName);
    int len = getMin(len1, len2, size);
    int firstIndex = 0, seconIndex = 0;
    for (int i = 0; i < len; i++)
    {
        c1 = tolower(fileArgName[seconIndex]);
        c2 = tolower(fileName[firstIndex]);
        if (isRegexPos(head, seconIndex, &prevChar))
        {
            while (c2 == prevChar)
            {
                firstIndex++;
                c2 = tolower(fileName[firstIndex]);
            }
            seconIndex++;
            c1 = tolower(fileArgName[seconIndex]);
            if (c1 != c2)
            {
                return 0;
            }
        }
        else
        {
            if (c1 != c2)
            {
                return 0;
            }
        }
        seconIndex++;
        firstIndex++;
    }
    return 1;
}
int checkFileSize(char *path,char *argSize){
    struct stat fileStat;
    if (stat(path, &fileStat) == -1){
        fprintf(stderr, "Stat system call error! %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    int givenArgSize = atoi(argSize);
    int fileSize = (int)fileStat.st_size;
    return fileSize == givenArgSize ? 1 : 0; 
}
int checkFileType();
int checkFilePermission();
int checkFileLinks();
void showSearchResults(int isFound);
void drawTree(char *targetPath, char *fileName);

int main(int argc, char *argv[])
{
    args a;
    checkArguments(argc, argv, &a);
    //printf("arg: %s\n",a.wArg);
    traversePathRecursively(a.wArg, a);
    //printf("count: %d\n",a.count);
    //printf("filename main: %s\n",a.fArg);
    return 0;
}