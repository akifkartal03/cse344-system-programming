#include "search.h"

void traversePathRecursively(char *targetPath, args *givenArgs)
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
            size_t pathLength = strlen(currentPath);
            if (pathLength >= PATH_MAX)
            {
                fprintf(stderr, "Path length is longer than expected!\n");
                exit(EXIT_FAILURE);
            }
            traversePathRecursively(currentPath, givenArgs); //call function with child directory
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
    if (stat(path, &fileStat) == -1)
    {
        fprintf(stderr, "Stat system call error! %s %s\n", path, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (givenArgs->fFlag)
    {
        if (checkFileName(fileName, givenArgs->fArg))
        {
            options++;
        }
    }
    if (givenArgs->bFlag)
    {

        if (checkFileSize(fileStat, givenArgs->bArg))
        {
            options++;
        }
    }
    if (givenArgs->tFlag)
    {
        if (checkFileType(fileStat, givenArgs->tArg))
        {
            options++;
        }
    }
    if (givenArgs->pFlag)
    {
        if (checkFilePermission(fileStat, givenArgs->pArg))
        {
            options++;
        }
    }
    if (givenArgs->lFlag)
    {

        if (checkFileLinks(fileStat, givenArgs->lArg))
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
int checkFileName(char *fileName, char *fileArgName)
{
    node_t *head = NULL;
    int size = 0;
    head = getRegexsPositions(head, fileArgName, &size);
    char prevChar, c1, c2;
    int len1 = strlen(fileArgName);
    int len2 = strlen(fileName);
    if (size == 0 && len1 != len2)
    {
        freeList(head);
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
                freeList(head);
                return 0;
            }
        }
        else
        {
            if (c1 == '\\')
            {
                if (seconIndex < len1 && tolower(fileArgName[seconIndex+1]) == '+')
                {
                    if (c2 != '+')
                    {
                        freeList(head);
                        return 0;
                    }
                    else
                        seconIndex++;
                    
                }
                else{
                    freeList(head);
                    return 0;
                }
                    
                
            }
            else{
                if (c1 != c2)
                {
                    freeList(head);
                    return 0;
                }
            }
        }
        seconIndex++;
        firstIndex++;
    }
    freeList(head);
    return 1;
}
int checkFileSize(struct stat fileStat, char *argSize)
{

    int givenArgSize = atoi(argSize);
    int fileSize = (int)fileStat.st_size;
    return fileSize == givenArgSize ? 1 : 0;
}
int checkFileType(struct stat fileStat, char *argType)
{

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
int checkFilePermission(struct stat fileStat, char *argPermissions)
{
    char filePermission[10] = "";

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
    if (strcmp(filePermission, argPermissions) == 0)
        return 1;
    else
        return 0;
}
int checkFileLinks(struct stat fileStat, char *argNumber)
{
    int givenArgNumber = atoi(argNumber);
    int numberOfLink = (int)fileStat.st_nlink;
    return numberOfLink == givenArgNumber ? 1 : 0;
}
void showSearchResults(int isFound, char *targetPath, args givenArgs)
{
    if (isFound)
    {
        printf("%s\n", targetPath);
        drawTree(targetPath, givenArgs, 2);
    }
    else
        printf("No file found\n");
}
void drawTree(char *targetPath, args givenArgs, int height)
{
    //use same recursive algorithm to draw tree
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

        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {

            printf("|");
            for (int i = 0; i < height; i++)
            {
                printf("-");
            }
            strcpy(currentPath, targetPath);
            strcat(currentPath, "/");
            strcat(currentPath, entry->d_name);
            if (checkGivenArguments(currentPath, &givenArgs, entry->d_name))
                printf(BOLDRED "%s\n" RESET, entry->d_name);
            else
                printf("%s\n", entry->d_name);
            size_t pathLength = strlen(currentPath);
            if (pathLength >= PATH_MAX)
            {
                fprintf(stderr, "Path length is longer than expected!\n");
                exit(EXIT_FAILURE);
            }
            drawTree(currentPath, givenArgs, height + 2);
        }
    }
    if (closedir(dir))
    {
        fprintf(stderr, "Directory close error! %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}