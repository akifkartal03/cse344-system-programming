#include "search.h"

int main(int argc, char *argv[])
{
    /*CTRL-C signal handling with sigaction*/
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &exitHandler;
    sigaction(SIGINT, &sa, NULL);

    //start search operation
    args givenParams;
    checkArguments(argc, argv, &givenParams);
    traversePathRecursively(givenParams.wArg, &givenParams);
    showSearchResults(givenParams.isFound, givenParams.wArg, givenParams);
    return 0;
}