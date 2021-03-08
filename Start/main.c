

#include "header.h"
int main(){
    
    return 0;
}

void createTree(int height){
    if (height == 0)
    {
        manageLeafNode();
        return;
    }
    int pid = fork();
    if (pid > 0)
    {
        //parent process
        int pid2 = fork();
        if (pid2 > 0)
        {
            //parent process
            manageParent(pid,pid2);

        }
        else if (pid2 == 0)
        {
            //left child
            createTree(--height);
        }
        
    }
    else if (pid == 0)
    {
        //right child
        createTree(--height);
    }
    
    
    
}
void manageLeafNode(){
    char filename[20];
    //make file name process id
    sprintf(filename,"%d.txt",getpid());
    //open file by using system call
    int fd = open(filename,O_RDONLY | O_CREAT);
    if (fd == -1){
        //denote system call error
        fprintf(stderr,"open file error!!\n");
        exit(EXIT_FAILURE);
    }
    //create random number
    srand(getpid());
    int rnd = rand()/100;
    size_t size = floor (log10 (abs (rnd))) + 1;
    int res = write(fd,rnd,size);
    if (res == -1)
    {
        fprintf(stderr,"write to file error");
        exit(EXIT_FAILURE);
    }
    if (close(fd) < 0)
    {
        fprintf(stderr,"write to file error");
        exit(EXIT_FAILURE);
    }

}