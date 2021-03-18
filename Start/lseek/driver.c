#include "main.h"

int main(int argc, char *argv[]){

    if (argc < 4)
    {
        showUsageandExit();
    }
    char *inFileName = argv[1];
    char *outputFile = argv[2];
    int n = atoi(argv[3]);
    copyNbyte(inFileName,outputFile,n);
    return 0;
}
void copyNbyte(char *iFile, char *oFile,int n){
    int fd_read = open(iFile,O_RDONLY,S_IRWXU);
    if (fd_read == -1)
    {
        perror("Failed to open input file");
        exit(EXIT_FAILURE);
    }
    int fd_write = open(oFile,O_WRONLY | O_CREAT,S_IRWXU);
    if (fd_write == -1)
    {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }
    char buf[32];
    int count = 0;
    int readbytes = 0;
    //read 1 byte

    while (read(fd_read,buf,1))
    {
        if(count < n){
            if (lseek(fd_read,n,SEEK_CUR) == -1)
            {
                perror("Failed to move cursor with lseek");
                exit(EXIT_FAILURE);
            }
            if (write(fd_write,buf,1) == -1){
                perror("Failed to write");
                exit(EXIT_FAILURE);
            }
            count = n;
            
        }
        else
        {
            count = 2*n;
            if (lseek(fd_read,count,SEEK_CUR) == -1)
            {
                perror("Failed to move cursor with lseek");
                exit(EXIT_FAILURE);
            }
            if (write(fd_write,buf,1) == -1){
                perror("Failed to write");
                exit(EXIT_FAILURE);
            }

        }
        
    }
   if(close(fd_read)==-1){
        perror("Failed to close");
        exit(EXIT_FAILURE);
   }
   if(close(fd_write)==-1){
        perror("Failed to close");
        exit(EXIT_FAILURE);
   }

}
void showUsageandExit(){
    printf("Usage: ./run input_file_name output_file_name number of bytes\n");
    exit(EXIT_FAILURE);
}
