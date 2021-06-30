#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<time.h>
#include<sys/types.h> 
#include<string.h>
#include<sys/wait.h>
#include <errno.h>
/* a shell program that will be very similar to 
*  the example given in the textbook in Fig 1-19. 
*/
#define TRUE 1
int pid;
char *argv[2];
char filename[41];

void type_prompt()
{
   //wait a command sign. 	
   printf("Shell> ");
}

void read_command(char *argv0, char *argv[])
{	
   scanf("%s",filename);
   argv[0] = filename;
   argv[1] = NULL;
   argv0 = argv[0];
}

int main()
{
   int status;
   argv[0] = NULL;
   while (TRUE) /* repeat forever */ {
      type_prompt(); /* display prompt on screen */
      read_command(argv[0],argv); /* read input from terminal */
      if ((pid = fork()) != 0) /* fork off child process */ {
          /* Parent code. */
         waitpid(-1, &status, 0); /* wait for child to exit */
      } 
      else {
          /* Child code. */
         if (execvp(argv[0], argv) == -1) /* execute command */
            printf("Exec error: %s\n", strerror(errno));
      }
   }
   return 0;
}