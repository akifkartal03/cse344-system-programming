#include "header.h"
 
int main(){
    createTree(3);
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
            //printf(" parent: %d , lchild:%d,rchild:%d, height:%d\n",getpid(),pid,pid2,height);
            wait(NULL);
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
    int fd = open(filename,O_WRONLY | O_CREAT,S_IRWXU);
    if (fd == -1){
        //denote system call error
        fprintf(stderr,"open file0 error!!\n");
        exit(EXIT_FAILURE);
    }
    //create random number
    srand(getpid());
    int *rnd = (int*)malloc(sizeof(int));
    *rnd = (rand() % 
           (99 - 10 + 1)) + 10; ;
    printf("rnd: %d\n",*rnd);
    int res = write(fd,rnd,2);
    if (res == -1)
    {
        fprintf(stderr,"write to file error");
        exit(EXIT_FAILURE);
    }
    if (close(fd) < 0)
    {
        fprintf(stderr,"close file error");
        exit(EXIT_FAILURE);
    }

}
void manageParent(int pid1, int pid2){
    wait(NULL);
    char filename1[20],filename2[20],filename3[20];
    //make file name process id
    sprintf(filename1,"%d.txt",pid1);
    sprintf(filename2,"%d.txt",pid2);
    sprintf(filename3,"%d.txt",getpid());
    int fd1 = open(filename1,O_RDONLY,S_IRWXU);
    if (fd1 ==-1) 
    { 
        // print which type of error have in a code 
        printf("Error Number % d\n", errno);  
          
        // print program detail "Success or failure" 
        perror("Program");                  
    } 
    int fd2 = open(filename2,O_RDONLY,S_IRWXU);
    if (fd2 == -1){
        //denote system call error
        fprintf(stderr,"open file2 error!!\n");
        exit(EXIT_FAILURE);
    }
    int *r = (int*)malloc(sizeof(int));
    int *l = (int*)malloc(sizeof(int));
    
    int rd = read(fd1, r, 2);

    if(rd == -1){
        fprintf(stderr, "read input file");
        exit(EXIT_FAILURE);
    }
    int rd2 = read(fd2, l, 2);
    if(rd2 == -1){
        fprintf(stderr, "read input file");
        exit(EXIT_FAILURE);
    }
    if (close(fd1) < 0)
    {
        fprintf(stderr,"close file error");
        exit(EXIT_FAILURE);
    }
    if (close(fd2) < 0)
    {
        fprintf(stderr,"close file error");
        exit(EXIT_FAILURE);
    }
    printf("r: %d\n",*r);
    printf("l: %d\n",*l);
    int fd3 = open(filename3,O_WRONLY | O_CREAT,S_IRWXU);
    if (fd3 == -1){
        //denote system call error
        fprintf(stderr,"open file3 error!!\n");
        exit(EXIT_FAILURE);
    }
    //char buf1[20];
    //make file name process id
    int *b = (int*)malloc(sizeof(int));
    *b = *l+*r;
    printf("b: %d\n",*b);
    if (*b > 99)
    {
        int res1 = write(fd3,b,3);
    }
    else{
        int res1 = write(fd3,b,2);
    }
    
    int res1 = write(fd3,b,2);
    if (res1 == -1)
    {
        fprintf(stderr,"write to file error");
        exit(EXIT_FAILURE);
    }
    if (close(fd3) < 0)
    {
        fprintf(stderr,"close file error");
        exit(EXIT_FAILURE);
    }
    free(l);
    free(r);

}