#include "helper.h"
volatile __sig_atomic_t exitSignal = 0;
void checkArguments(int argc, char **argv, args *givenArgs)
{

    int opt; //option
    //initialize all flags to zero
    givenArgs->count = 0;
    givenArgs->sFlag = 0;
    givenArgs->fFlag = 0;
    givenArgs->bFlag = 0;
    givenArgs->mFlag = 0;
    if (argc < 9)
    {
        showUsageAndExit();
    }
    while ((opt = getopt(argc, argv, "b:s:f:m:")) != -1)
    {
        switch (opt)
        {
        case 'b':
            givenArgs->bFlag = 1;
            givenArgs->bArg = optarg;
            break;
        case 's':
            givenArgs->sFlag = 1;
            givenArgs->sArg = optarg;
            break;
        case 'f':
            givenArgs->fFlag = 1;
            givenArgs->fArg = optarg;
            break;
        case 'm':
            givenArgs->mFlag = 1;
            givenArgs->mArg = optarg;

            break;
        case '?':
            showUsageAndExit();
            break;
        default:
            showUsageAndExit();
            break;
        }
        if (exitSignal)
        {
            printf("You are exiting...\n");
            exit(EXIT_SUCCESS);
        }
        
    }
}

void showUsageAndExit()
{
    printf("Usage: ./player [FLAGS] and [PARAMETERS]\n"
            "Flags and Parameters:\n"
            "-b : haspotatoornot, can be 0 or a positive integer. \n"
            "-s : nameofsharedmemory, this will be the name of the posix named shared memory\n"
            "-f: filewithfifonames ,this ascii file will contain the names of the fifoes\n"
            "-m: namedsemaphore, named posix semaphore to be used for synchronization\n"
            "Example\n"
            "./player –b 5 –s shr_mem –f fifo_names –m sem\n")
    exit(EXIT_FAILURE);
}
int getRandom(int n){
    srand(time(0));
    return (rand() % (n - 2)) + 1;
}
void exitHandler(int signal)
{
    if (signal == SIGINT)
    {
        exitSignal = 1;
    }
}