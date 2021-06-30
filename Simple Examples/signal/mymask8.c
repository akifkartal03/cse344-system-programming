#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>

struct process
{
    struct process *next;
    /* The process ID of this child.  */
    int pid;
    /* The descriptor of the pipe or pseudo terminal
     on which output comes from this child.  */
    int input_descriptor;
    /* Nonzero if this process has stopped or terminated.  */
    sig_atomic_t have_status;
    /* The status of this child; 0 if running,
     otherwise a status value from waitpid.  */
    int status;
};

struct process *process_list;


    /* Nonzero means some child's status has changed
   so look at process_list for the details.  */
int process_status_change;

void sigchld_handler(int signo)
{
    int old_errno = errno;

    while (1)
    {
        register int pid;
        int w;
        struct process *p;

        /* Keep asking for a status until we get a definitive result.  */
        do
        {
            errno = 0;
            pid = waitpid(WAIT_ANY, &w, WNOHANG | WUNTRACED);
        } while (pid <= 0 && errno == EINTR);

        if (pid <= 0)
        {
            /* A real failure means there are no more
         stopped or terminated child processes, so return.  */
            errno = old_errno;
            return;
        }

        /* Find the process that signaled us, and record its status.  */

        for (p = process_list; p; p = p->next)
            if (p->pid == pid)
            {
                p->status = w;
                /* Indicate that the status field
           has data to look at.  We do this only after storing it.  */
                p->have_status = 1;

                /* If process has terminated, stop waiting for its output.  */
                if (WIFSIGNALED(w) || WIFEXITED(w))
                    if (p->input_descriptor)
                        FD_CLR(p->input_descriptor, &input_wait_mask);

                /* The program should check this flag from time to time
           to see if there is any news in process_list.  */
                ++process_status_change;
            }

        /* Loop around to handle all the processes
       that have something to tell us.  */
    }
}

if (process_status_change)
{
    struct process *p;
    process_status_change = 0;
    for (p = process_list; p; p = p->next)
        if (p->have_status)
        {
            ... Examine p->status...
        }
}