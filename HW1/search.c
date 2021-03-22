#include "search.h"
#include "helper.c"

void traversePathRecursively(args *givenArgs)
{
    char currentPath[PATH_MAX];
    DIR *dir;
    struct dirent *entry;
    size_t pathLength = strlen(givenArgs->wArg);

    if (pathLength >= PATH_MAX)
    {
        fprintf(stderr, "Path length is longer than expected!\n");
        exit(EXIT_FAILURE);
    }
    dir = opendir(givenArgs->wArg);
    if (!dir)
    {
        return;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        //check directory is different than current and parent
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {

            strcpy(currentPath, givenArgs->wArg);
            //printf("curr: %s\n",currentPath);
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
            givenArgs->wArg = currentPath;
            traversePathRecursively(givenArgs);
        }
    }
    if (closedir(dir))
    {
        fprintf(stderr, "Directory close error! %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}
int checkGivenArguments(char *path, args *givenArgs, char *fileName)
{
    
    int options = 0;
    struct stat fileStat;
    if (stat(path, &fileStat) == -1){
        fprintf(stderr, "Stat system call error! %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (givenArgs->fFlag)
    {
        if (checkFileName(fileName, givenArgs->fArg, path))
        {
            options++;
            printf("%s\n",givenArgs->fArg);
        }
    }
    if (givenArgs->bFlag)
    {
        
        if (checkFileSize(fileStat,givenArgs->bArg))
        {
            options++;
        }
        
    }
    if (givenArgs->tFlag)
    {
        if (checkFileType(fileStat,givenArgs->tArg))
        {
            options++;
            //printf("asfsafsdf\n");
        }
        
    }
    if (givenArgs->pFlag)
    {
        if (checkFilePermission(fileStat,givenArgs->pArg))
        {
            options++;
            //printf("asfsafsdf\n");
        }
        
    }
    if (givenArgs->lFlag)
    {
        
        if (checkFileLinks(fileStat,givenArgs->lArg))
        {
            options++;
        }
        
    }
    if (givenArgs->count == options)
    {
       givenArgs->isFound = 1;
       return 1;
    }
    return 0;
    
}
int checkFileName(char *fileName, char *fileArgName, char *path)
{
    node_t *head = NULL;
    int size = 0;
    head = getRegexsPositions(head, fileArgName, &size);
    char prevChar, c1, c2;
    int len1 = strlen(fileArgName);
    int len2 = strlen(fileName);
    if (size == 0 && len1!=len2)
    {
        return 0;
    }
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
int checkFileSize(struct stat fileStat,char *argSize){
   
    int givenArgSize = atoi(argSize);
    int fileSize = (int)fileStat.st_size;
    return fileSize == givenArgSize ? 1 : 0; 
}
int checkFileType(struct stat fileStat,char *argType){
   
    if (S_ISLNK(fileStat.st_mode) && *argType == 'l')
        return 1;
    else if (S_ISDIR(fileStat.st_mode) && *argType == 'd')
        return 1;
    else if (S_ISCHR(fileStat.st_mode) && *argType == 'c')
        return 1;
    else if (S_ISBLK(fileStat.st_mode) && *argType == 'b')
        return 1;
    else if (S_ISFIFO(fileStat.st_mode) && *argType == 'p')
        return 1;
    else if (S_ISSOCK(fileStat.st_mode) && *argType == 's')
        return 1;
    else if (S_ISREG(fileStat.st_mode) && *argType == 'f')
        return 1;
    else
        return 0;
    
}
int checkFilePermission(struct stat fileStat,char *argPermissions){
    char filePermission[10];
    
    //user permissions
    (fileStat.st_mode & S_IRUSR) ? strcat(filePermission, "r") : strcat(filePermission, "-");
    (fileStat.st_mode & S_IWUSR) ? strcat(filePermission, "w") : strcat(filePermission, "-"); 
    (fileStat.st_mode & S_IXUSR) ? strcat(filePermission, "x") : strcat(filePermission, "-");
    //group permissions
    (fileStat.st_mode & S_IRGRP) ? strcat(filePermission, "r") : strcat(filePermission, "-");
    (fileStat.st_mode & S_IWGRP) ? strcat(filePermission, "w") : strcat(filePermission, "-");
    (fileStat.st_mode & S_IXGRP) ? strcat(filePermission, "x") : strcat(filePermission, "-"); 
    //other permissions
    (fileStat.st_mode & S_IROTH) ? strcat(filePermission, "r") : strcat(filePermission, "-");
    (fileStat.st_mode & S_IWOTH) ? strcat(filePermission, "w") : strcat(filePermission, "-");
    (fileStat.st_mode & S_IXOTH) ? strcat(filePermission, "x") : strcat(filePermission, "-"); 
    strcat(filePermission, "\0");
    if (strcmp(filePermission,argPermissions) == 0)
        return 1;
    else
        return 0;

}
int checkFileLinks(struct stat fileStat,char *argNumber){
    int givenArgNumber = atoi(argNumber);
    int numberOfLink = (int)fileStat.st_nlink;
    //printf("file: %d\n"numberOfLink);
    //printf("arg: %d\n"givenArgNumber);
    return numberOfLink == givenArgNumber ? 1 : 0;
}
void showSearchResults(int isFound,char *targetPath){
    if (isFound)
    {
        printf("huraaaaa\n");
    }
    
}
void drawTree(char *targetPath, char *fileName);

int main(int argc, char *argv[])
{
    args a;
    checkArguments(argc, argv, &a);
    char *targetPath = a.wArg;
    traversePathRecursively(&a);
    showSearchResults(a.isFound);
    //printf("count: %d\n",a.count);
    //printf("filename main: %s\n",a.fArg);
    return 0;
}