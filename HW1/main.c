#include "search.h"

int main(int argc, char *argv[])
{
    signal(SIGINT, exitHandler);
    args givenParams;
    checkArguments(argc, argv, &givenParams);
    traversePathRecursively(givenParams.wArg, &givenParams);
    //give a message or draw tree
    showSearchResults(givenParams.isFound, givenParams.wArg, givenParams);
    return 0;
}