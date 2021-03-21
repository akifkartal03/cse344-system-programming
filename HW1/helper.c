#include "helper.h"

void checkArguments(const int argc, const char **argv,args *givenArgs)
{

    int opt;       //option
    int wFlag2 = 0; //flags
    int count = 0;
    if (argc < 5)
    {
        showUsageAndExit();
    }
    while ((opt = getopt(argc, argv, "w:f:b:t:p:l:")) != -1)
    {
        switch (opt)
        {
        case 'w':
            wFlag2 = 1;
            printf("w: %s\n", optarg);
            break;
        case 'f':
            if (wFlag2)
            {
                count++;
                printf("f: %s\n", optarg);
            }

            break;
        case 'b':
            if (wFlag2)
            {
                count++;
                printf("b: %s\n", optarg);
            }

            break;
        case 't':
            if (wFlag2)
            {
                count++;
                printf("t: %s\n", optarg);
            }

            break;
        case 'p':
            if (wFlag2)
            {
                count++;
                printf("p: %s\n", optarg);
            }

            break;
        case 'l':
            if (wFlag2)
            {
                count++;
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
    if (wFlag2 == 0 || count == 0)
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

    checkArguments(argc, argv);
    return 0;
}*/
