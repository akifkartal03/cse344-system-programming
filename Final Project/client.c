#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include "helper.h"
#include "sql_engine.h"
#define MAX 4096
#define MAX_SEND 1024
#define read 1
#define write 2

typedef struct cArgs
{
    int id;
    char ipAdr[41];
    int clientPort;
    int queryFd;
} clientArg;
typedef struct d{
    char *query;
}data;
void checkClientArguments(int argc, char **argv, clientArg *givenArgs);
void showClientUsageAndExit();
char *getLine();
int isMyLine(char *line);
clientArg givenParams;
int main(int argc, char *argv[])
{

    checkClientArguments(argc,argv,&givenParams);


    int clientSocket;
    struct sockaddr_in serverAddr;

    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        errExit("socket error!",0);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(givenParams.ipAdr);
    serverAddr.sin_port = htons(givenParams.clientPort);

    printf("[%s] Client-%d connecting to %s:%d\n", getTime(), givenParams.id, givenParams.ipAdr, givenParams.clientPort);
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) != 0)
            errExit( "connect error!",0);
    int count = 0;
    char *query1 = getLine();
    while(query1 != NULL){
        printf("[%s] Client-%d connected and sending query %s\n", getTime(), givenParams.id, query1);

        clock_t start, end;
        int first = 1 ,isFinished = 0;
        int res;
        char *response = (char*) calloc(MAX + 1,sizeof(char));

        safeWrite(clientSocket, query1, strlen(query1),0);
        start = clock();
        res = safeRead(clientSocket,response,MAX,0);
        end = clock();
        do{
            if(res<=0)
                errExit("client read error!",0);

            if (response[strlen(response)-1] == '\'' && response[strlen(response)-2] == '\"'){
                if(first){
                    printf("[%s] Server’s response to Client-%d is %d records, and arrived in %.5f seconds\n",
                           getTime(), givenParams.id, getReturnSize(response),((double)(end - start) / CLOCKS_PER_SEC));
                    char *pos = strstr(response,"\n");
                    pos++;
                    printData(pos);
                }
                else{
                    printData(response);
                }
                isFinished = 1;
               break;
            }
            if(first){
                printf("[%s] Server’s response to Client-%d is %d records, and arrived in %.5f seconds\n",
                       getTime(), givenParams.id, getReturnSize(response),(double)(end - start) / CLOCKS_PER_SEC);
                char *pos = strstr(response,"\n");
                pos++;
                printf("%s",pos);

            }
            if(!first)
                printf("%s",response);
            else
                first = 0;

        } while((res = safeRead(clientSocket,response,MAX,0)) == MAX);
        if(!isFinished)
            printData(response);
        //printf("%s",response);
        //printf("res:%d\n",res);
        //safeRead(clientSocket,response,MAX,0);
        //printf("%s",response);
        /*
        if(getQueryTypeEngine(query1) == read){
            printf("[%s] Server’s response to Client-%d is %d records, and arrived in %.5f seconds\n",
                   getTime(), givenParams.id, getReturnSize(resp),(double)(end - start) / CLOCKS_PER_SEC);
            printData(resp);
        }
        else{
            printf("[%s] Server’s response to Client-%d is %d records affected, and arrived in %.5f seconds\n",
                   getTime(), givenParams.id, atoi(resp),(double)(end - start) / CLOCKS_PER_SEC);
        }*/
        free(response);
        count++;
        query1 = getLine();
    }
    //char exit[10] = "exit";
    //safeWrite(clientSocket, exit, sizeof(exit),0);
    printf("[%s] A total of %d queries were executed, client is terminating.\n",getTime(),count);
    close(clientSocket);
    //printf("[%s] response size:%d\n",getTime(),response.size);
    return 0;
}



void checkClientArguments(int argc, char **argv, clientArg *givenArgs){
    if (argc != 9)
    {
        showClientUsageAndExit();
    }
    int opt,res;
    while ((opt = getopt(argc, argv, "i:a:p:o:")) != -1)
    {
        switch (opt)
        {
            case 'i':
                res = atoi(optarg);
                if (res <= 0)
                {
                    fprintf(stderr, "ID should be bigger than 0!\n");
                    showClientUsageAndExit();
                }
                givenArgs->id = res;
                break;
            case 'a':
                strcpy(givenArgs->ipAdr,optarg);
                break;
            case 'p':
                res = atoi(optarg);
                if (res <= 1000)
                {
                    fprintf(stderr, "port should be bigger than 1000!\n");
                    showClientUsageAndExit();
                }
                givenArgs->clientPort = res;
                break;
            case 'o':
                givenArgs->queryFd = safeOpen(optarg, O_RDONLY,0);
                safeLseek(givenArgs->queryFd, 0, SEEK_SET,0);
                break;
            case '?':
                showClientUsageAndExit();
                break;
            default:
                showClientUsageAndExit();
                break;
        }
    }
}
void showClientUsageAndExit(){
    printf("Usage: ./client [FLAGS] and [PARAMETERS]\n"
           "Flags and Parameters:\n"
           "-i: integer id of the client (>=1) \n"
           "-a: IPv4 address of the machine running the server.\n"
           "-p: port number at which the server waits for connections (>1000)\n"
           "-o: relative or absolute path of the file containing an arbitrary number of queries.\n"
           "Example\n"
           "./client -i 3 -a 127.0.0.1 -p 3456 -o /home/akif/sysprog/queries.txt\n");
    exit(EXIT_FAILURE);
}
int isMyLine(char *line){
    char temp[strlen(line) + 1];
    strcpy(temp,line);
    char *num = strtok(temp," ");
    if(num != NULL) {
        if (atoi(num) == givenParams.id) {
            return 1;
        }
    }
    return 0;
}

char *getLine(){

    int offset = 0;
    int bytes_read;
    int capacity = 50;
    int i = 0;
    char c;
    char *buffer = (char *)calloc(50, sizeof(char));
    do
    {
        bytes_read = safeRead(givenParams.queryFd, &c, 1,0);
        offset += bytes_read;
        if (capacity <= offset + 1)
        {
            capacity = capacity + 20;
            buffer = realloc(buffer, capacity * sizeof(char));
        }

        if (c == '\n')
        {
            if(bytes_read != 0){
                if(isMyLine(buffer)){
                    buffer[i] = '\0';
                    char *test = strstr(buffer," ");
                    test++;
                    //free(buffer);
                    return test;
                }
                else{
                    free(buffer);
                    buffer = (char *)calloc(50, sizeof(char));
                    capacity = 50;
                    offset = 0;
                    i = 0;
                }
            }
        }
        else{
            if(bytes_read != 0){
                buffer[i] = c;
                i++;
            }
            else{
                if(isMyLine(buffer)){
                    buffer[i] = '\0';
                    char *test = strstr(buffer," ");
                    test++;
                    //free(buffer);
                    return test;
                }
            }
        }
    } while (bytes_read == 1);
    free(buffer);
    return NULL;
}