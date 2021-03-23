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
void showUsageAndExit()
{
    // All error messages are to be printed to stderr.
    fprintf(stderr, "Usage: ./myFind [FLAGS] and [PARAMETERS]\n"
                    "Optional Flags: in any combinations(at least 1)\n"
                    "-f : filename (case insensitive), supporting the following regular expression: + \n"
                    "-b : file size (in bytes) \n"
                    "-t : file type (d: directory, s: socket, b: block device, c: character device f: regular file, p: pipe, l: symbolic link) \n"
                    "-p : permissions, as 9 characters (e.g. ‘rwxr-xr--’) -l: number of links \n"
                    "Mandotory Flags:\n"
                    "-w: the path in which to search recursively (i.e. across all of its subtrees)\n"
                    "Example\n"
                    "./myFind -w targetDirectoryPath -f ‘lost+file‘ -b 100 -t b\n");
    exit(EXIT_FAILURE);
}
node_t* getRegexsPositions(node_t *head, char *str,int *size)
{
    
    int len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        if (str[i] == '+' && i!=0)
        {
            head = addLast(head,str[i-1],i);
            *size = *size + 1;
        }
    }
    return head;
}
int isRegexPos(node_t *head, int pos,char *prevChar){
    node_t *iter;
    iter = head;
    while (iter!=NULL)
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
int getMin(int n1,int n2, int size){
   return n1 < n2 ? (n1 - size) : n2;   
}
void freeList(node_t *head){
    node_t *iter;
    while (head != NULL)
    {
       iter = head;
       head = head->next;
       free(iter);
    }
}
/*int main(int argc, char *argv[])
{
    //args a;
    //checkArguments(argc, argv,&a);
    //printf("count: %d\n",a.count);
    // printf("filename main: %s\n",a.fArg);

    node_t *head = NULL;
    char buf[20] = "file+sytem+test+";
    head = getRegexsPositions(head,buf);
    node_t *iter;
    iter = head;
    while (iter!=NULL)
    {
        printf("position: %d\n",iter->position);
        printf("prev char: %c\n",iter->preChr);
        iter = iter->next;
    }
    
    return 0;
}*/
