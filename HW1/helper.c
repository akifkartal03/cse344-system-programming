#include "helper.h"

void checkArguments(int argc, char **argv, args *givenArgs)
{

    int opt;       //option
    //initialize all flags to zero
    givenArgs->count = 0;
    givenArgs->wFlag = 0;
    givenArgs->fFlag = 0;
    givenArgs->bFlag = 0;
    givenArgs->tFlag = 0;
    givenArgs->pFlag = 0;
    givenArgs->lFlag = 0;
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
            printf("w: %s\n", optarg);
            break;
        case 'f':
            if (givenArgs->wFlag)
            {
                givenArgs->count++;
                givenArgs->fArg = optarg;
                givenArgs->fFlag =1;
                printf("f: %s\n", optarg);
            }

            break;
        case 'b':
            if (givenArgs->wFlag)
            {
                givenArgs->count++;
                givenArgs->bArg = optarg;
                givenArgs->bFlag = 1;
                printf("b: %s\n", optarg);
            }

            break;
        case 't':
            if (givenArgs->wFlag)
            {
                givenArgs->count++;
                givenArgs->tArg = optarg;
                givenArgs->tFlag = 1;
                printf("t: %s\n", optarg);
            }

            break;
        case 'p':
            if (givenArgs->wFlag)
            {
                givenArgs->count++;
                givenArgs->pArg = optarg;
                givenArgs->pFlag = 1;
                printf("p: %s\n", optarg);
            }

            break;
        case 'l':
            if (givenArgs->wFlag)
            {
                givenArgs->count++;
                givenArgs->lArg = optarg;
                givenArgs->lFlag = 1;
                printf("l: %s\n", optarg);
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
    fprintf(stderr, "Usage: ./search [FLAGS] and [PARAMETERS]\n"
                    "Optional Flags: in any combinations(at least 1)\n"
                    "-f : filename (case insensitive), supporting the following regular expression: + \n"
                    "-b : file size (in bytes) \n"
                    "-t : file type (d: directory, s: socket, b: block device, c: character device f: regular file, p: pipe, l: symbolic link) \n"
                    "-p : permissions, as 9 characters (e.g. ‘rwxr-xr--’) -l: number of links \n"
                    "Mandotory Flags:\n"
                    "-w: the path in which to search recursively (i.e. across all of its subtrees)\n"
                    "Example\n"
                    "./search -w targetDirectoryPath -f ‘lost+file‘ -b 100 -t b\n");
    exit(EXIT_FAILURE);
}

/*int main(int argc, char *argv[])
{
    args a;
    checkArguments(argc, argv,&a);
    printf("count: %d\n",a.count);
    printf("filename main: %s\n",a.fArg);
    return 0;
}*/
