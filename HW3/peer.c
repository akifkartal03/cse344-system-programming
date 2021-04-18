#include "helper.h"

int main(int argc, char *argv[])
{
    /*CTRL-C signal handling with sigaction*/
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &exitHandler;
    sigaction(SIGINT, &sa, NULL);
  
    args givenParams;
    checkArguments(argc, argv, &givenParams);
    int myFd, dummyFd, reciverFd;
    //char clientFifo[CLIENT_FIFO_NAME_LEN];
    struct sender req;
    struct reciever resp;

    



    return 0;
}
