#include "search.h"

int main(int argc, char *argv[])
{
    signal(SIGINT, exitHandler);
    args a, b;
    checkArguments(argc, argv, &a);
    char *targetPath = a.wArg;
    //printf("target main: %s\n",targetPath);
    b = a;
    //printf("count: %d\n",b.count);
    //printf("filename main: %s\n",b.fArg);
    //printf("path main1: %s\n",b.wArg);
    traversePathRecursively(a.wArg,&a);
    showSearchResults(a.isFound, targetPath, b);
    //printf("path main2: %s\n",b.wArg);
    //printf("path main3: %s\n",a.wArg);
    //printf("count: %d\n",a.count);
    //printf("filename main: %s\n",a.fArg);
    return 0;
}