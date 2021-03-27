#include "test3.c"
int main()
{

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = &sig_handler;
    sigaction(SIGINT, &sa, NULL);
    //signal(SIGINT, sig_handler);
    // Directory path to list files
    char path[100];

    // Input path from user
    printf("Enter path to list files: ");
    scanf("%s", path);

    listFilesRecursively(path);

    return 0;
}