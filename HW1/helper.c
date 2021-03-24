#include "helper.h"

void checkArguments(int argc, char **argv, args *givenArgs)
{

    int opt; //option
    //initialize all flags to zero
    givenArgs->count = 0;
    givenArgs->wFlag = 0;
    givenArgs->fFlag = 0;
    givenArgs->bFlag = 0;
    givenArgs->tFlag = 0;
    givenArgs->pFlag = 0;
    givenArgs->lFlag = 0;
    givenArgs->isFound = 0;
    if (argc < 5)
    {
        showUsageAndExit();
    }
    while ((opt = getopt(argc, argv, "w:f:b:t:p:l:")) != -1)
    {
        switch (opt)
        {
        case 'w':
            givenArgs->wFlag = 1;
            givenArgs->wArg = optarg;
            break;
        case 'f':
            if (givenArgs->wFlag)
            {
                givenArgs->count++;
                givenArgs->fArg = optarg;
                givenArgs->fFlag = 1;
            }

            break;
        case 'b':
            if (givenArgs->wFlag)
            {
                givenArgs->count++;
                givenArgs->bArg = optarg;
                givenArgs->bFlag = 1;
            }

            break;
        case 't':
            if (givenArgs->wFlag)
            {
                givenArgs->count++;
                givenArgs->tArg = optarg;
                givenArgs->tFlag = 1;
            }

            break;
        case 'p':
            if (givenArgs->wFlag)
            {
                givenArgs->count++;
                givenArgs->pArg = optarg;
                givenArgs->pFlag = 1;
            }

            break;
        case 'l':
            if (givenArgs->wFlag)
            {
                givenArgs->count++;
                givenArgs->lArg = optarg;
                givenArgs->lFlag = 1;
            }
            break;
        case '?':
            showUsageAndExit();
            break;
        default:
            showUsageAndExit();
            break;
        }
    }
    if (givenArgs->wFlag == 0 || givenArgs->count == 0)
    {
        showUsageAndExit();
    }
}
void my_printf(const char *str){
    ssize_t size = strlen(str);
    if (size != write(STDOUT_FILENO, str, size)) {
        my_fprintf_with_stderr("write system call error");
    }
}
void my_fprintf_with_stderr(const char *str){
    ssize_t size = strlen(str);
    if (size != write(STDERR_FILENO, str, size)) {
        perror("write system call error!");
        exit(EXIT_FAILURE);
    }

}
void showUsageAndExit()
{
    // All error messages are to be printed to stderr.
    char *line1 = "Usage: ./myFind [FLAGS] and [PARAMETERS]\n";
    char *line2 = "Optional Flags: in any combinations(at least 1)\n";
    char *line3 = "-f : filename (case insensitive), supporting the following regular expression: + \n";
    char *line4 = "-b : file size (in bytes) \n";
    char *line5 = "-t : file type (d: directory, s: socket, b: block device, c: character device f: regular file, p: pipe, l: symbolic link) \n";
    char *line6 = "-p : permissions, as 9 characters (e.g. ‘rwxr-xr--’) -l: number of links \n";
    char *line7 = "Mandotory Flags:\n";
    char *line8 = "-w: the path in which to search recursively (i.e. across all of its subtrees)\n";
    char *line9 = "Example\n";
    char *line10 = "./myFind -w targetDirectoryPath -f ‘lost+file‘ -b 100 -t b\n";
    my_fprintf_with_stderr(line1);
    my_fprintf_with_stderr(line2);
    my_fprintf_with_stderr(line3);
    my_fprintf_with_stderr(line4);
    my_fprintf_with_stderr(line5);                
    my_fprintf_with_stderr(line6);
    my_fprintf_with_stderr(line7);
    my_fprintf_with_stderr(line8);
    my_fprintf_with_stderr(line9);
    my_fprintf_with_stderr(line10);
    exit(EXIT_FAILURE);
}
node_t *getRegexsPositions(node_t *head, char *str, int *size)
{

    int len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        if (str[i] == '+' && i != 0 && str[i - 1] != '\\')
        {
            head = addLast(head, str[i - 1], i);
            *size = *size + 1;
        }
    }
    return head;
}
int isRegexPos(node_t *head, int pos, char *prevChar)
{
    node_t *iter;
    iter = head;
    while (iter != NULL)
    {
        if (iter->position == pos)
        {
            *prevChar = iter->preChr;
            return 1;
        }
        iter = iter->next;
    }
    return 0;
}
int getMin(int n1, int n2, int size)
{
    return n1 < n2 ? (n1 - size) : n2;
}
void freeList(node_t *head)
{
    node_t *iter;
    while (head != NULL)
    {
        iter = head;
        head = head->next;
        free(iter);
    }
}
void exitHandler(int signal)
{
    if (signal == SIGINT)
    {
        printf("You are exiting...\n");
        exit(0);
    }
}
