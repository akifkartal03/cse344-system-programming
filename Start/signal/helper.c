//Ctrl+K+C, you'll comment out
//Ctrl+K+U will uncomment the code.
// int main(int argc, char *argv[])
// {
//     int offset = 0;
//     int bytes_read;
//     int capacity = 0;
//     checkArgument(argc);
//     char *path = argv[1];
//     int i = 0;
//     char c;
//     int fd = safeOpen(path, O_RDWR);
//    /* do
//     {
        
//         bytes_read = safeRead(fd, &c, 1); 
//         offset += bytes_read;
//         if (capacity <= offset + 1)
//         {
//             capacity = capacity + 100;
//             buffer = realloc(buffer, capacity * sizeof(char));
//         }
//         buffer[i] = c;
//         i++;
        
//     } while (bytes_read == 1); */
//     /*i = 0;
//     while (read(fd, &c, 1) == 1)
//     {
//         buffer[i] = c;
//         i++;
//         /* end of line
//         if (c == '\n')
//         {
//             buffer[i] = 0;
//             if (!strncmp(buffer, w, strlen(w)))
//             {
//                 printf("w was found in line %d\n", line);
//                 puts(buffer);
//                 n++;
//             }
//             line++;
//             i = 0;
//             continue;
//         }
//         i++; *
//     }*/
//     /*char *pt;
//     pt = strtok (buffer,",");
//     while (pt != NULL) {
//         double a = atof(pt);
//         printf("%.1f\n", a);
//         pt = strtok (NULL, ",");
//     }*/
//     //printf("%s\n",buffer);
//     //testLagrange(buffer,4);
//     /* All done.*/
//     //close(fd);
//     //int fd2=safeOpen(path, O_RDWR | O_APPEND);
//     //writeEndofLine(fd2,8.0,1);

//     //free(buffer);
//     pid_t pid;
//     int j, sigCnt;
//     sigset_t blockMask, emptyMask, blockMask2, emptyMask2,blockMask3, emptyMask5;
//     struct sigaction sa, sa2,sa4;

//     setbuf(stdout, NULL); 

//     sigCnt = 0;
//     numLiveChildren = 8;

//     sigemptyset(&sa.sa_mask);
//     sa.sa_flags = 0;
//     sa.sa_handler = &sigchldHandler;
//     if (sigaction(SIGCHLD, &sa, NULL) == -1)
//         errExit("sigaction");

//     sigemptyset(&sa2.sa_mask);
//     sa2.sa_flags = 0;
//     sa2.sa_handler = &handler;
//     if (sigaction(SIGUSR1, &sa2, NULL) == -1)
//         errExit("sigaction");
    
//     sigemptyset(&sa4.sa_mask);
//     sa4.sa_flags = 0;
//     sa4.sa_handler = &handler;
//     if (sigaction(SIGUSR2, &sa4, NULL) == -1)
//         errExit("sigaction");

//     sigemptyset(&blockMask);
//     sigaddset(&blockMask, SIGCHLD);
//     if (sigprocmask(SIG_SETMASK, &blockMask, NULL) == -1)
//         errExit("sigprocmask");

//     sigemptyset(&blockMask2);
//     sigaddset(&blockMask2, SIGUSR1);
//     if (sigprocmask(SIG_SETMASK, &blockMask2, NULL) == -1)
//         errExit("sigprocmask");

    
//     sigemptyset(&blockMask3);
//     sigaddset(&blockMask3, SIGUSR2);
//     if (sigprocmask(SIG_SETMASK, &blockMask3, NULL) == -1)
//         errExit("sigprocmask");

//     for (j = 1; j < 9; j++)
//     {
//         pid = fork();
//         if (pid == -1)
//         {
//             errExit("fork");
//         }
//         else if (pid == 0)
//         {
           
//             struct sigaction sa3;
//             sigemptyset(&sa3.sa_mask);
//             sa3.sa_flags = 0;
//             sa3.sa_handler = &handler2;
//             if (sigaction(SIGUSR1, &sa3, NULL) == -1)
//                 errExit("sigaction");
//             /*wait first signal*/
            
//             /*sigemptyset(&emptyMask3);
//             if (sigsuspend(&emptyMask3) == -1 && errno != EINTR)
//                 errExit("sigsuspend");*/
//             char *buffer = (char *)calloc(100, sizeof(char));    
//             printf("Child %d (PID=%ld) start to working round 1...\n", j, (long)getpid());
//             do
//             {

//                 bytes_read = safeRead(fd, &c, 1);
//                 offset += bytes_read;
//                 if (capacity <= offset + 1)
//                 {
//                     capacity = capacity + 100;
//                     buffer = realloc(buffer, capacity * sizeof(char));
//                 }
//                 buffer[i] = c;
//                 i++;

//             } while (bytes_read == 1);
//             //printf("Child %d (PID=%ld)\n%s\n",j, (long)getpid(),buffer);
//             //testLagrange(buffer,4);
//             safeLseek(fd, 1, SEEK_SET);
//             safeLseek(fd, -1, SEEK_CUR);
//             printf("Child %d (PID=%ld) suspending...\n", j, (long)getpid());
//             kill(getppid(), SIGUSR1);
//             /*Signal to the parent*/
//             /*if (j == 8)
//             {
//                 printf("Child %d (PID=%ld) signaling to parent...\n", j, (long)getpid());
//                 kill(getppid(), SIGUSR1);
//             }*/
//             /*wait second signal*/
//             sigset_t emptyMask3;
//             sigemptyset(&emptyMask3);
//             if (sigsuspend(&emptyMask3) == -1 && errno != EINTR)
//                 errExit("sigsuspend");

//             printf("Child (PID=%ld) GOT the signal...\n", (long)getpid());
//             writeEndofLine(fd,15.0,j,buffer);
//             free(buffer);
//             exit(EXIT_SUCCESS);
//         }
//         else
//         {
//             /*Parent Process*/
//             arr[j - 1] = pid;
//             sigemptyset(&emptyMask2);
//             if (sigsuspend(&emptyMask2) == -1 && errno != EINTR)
//                 errExit("sigsuspend");
//             //printf("signal to the... (PID=%ld)\n", (long)pid);
//             //kill(pid,SIGUSR1);
//             //printf("j: %d\n",j);
//             //counter--;
//         }
//     }
//     /*END OF FOR LOOP
//     sigemptyset(&emptyMask2);
//     if (sigsuspend(&emptyMask2) == -1 && errno != EINTR)
//         errExit("sigsuspend");*/
//     printf("Parent[%ld] has finished its waitng for round1..\n", (long)getpid());
//     //printf("counter: %d\n",counter);
//     /*Calculate firs errrorr!!!*/
//     for (int i = 0; i < 8; i++)
//     {
//         printf("Child[%ld]\n", (long)arr[i]);
//         kill(arr[i], SIGUSR1);
//         sigemptyset(&emptyMask2);
//         if (sigsuspend(&emptyMask2) == -1 && errno != EINTR)
//             errExit("sigsuspend");
//     }
//     /*SECOUNd*/
//     sigemptyset(&emptyMask);
//     while (numLiveChildren > 0)
//     {
//         if (sigsuspend(&emptyMask) == -1 && errno != EINTR)
//             errExit("sigsuspend");
//         sigCnt++;
//     }
//     printf("Parent (PID=%ld) exiting 2.error calculation\n", (long)getpid());
//     printf("All children have terminated; SIGCHLD was caught "
//            "%d times\n",
//            sigCnt);
//     close(fd);
//     return 0;
// }
