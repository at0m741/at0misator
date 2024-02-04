#include "debug.h"

int attach_and_trace(pid_t child_pid) 
{
    child_pid = fork();

    if (child_pid == -1) 
    {
        perror("fork");
        return -1;
    }
    if (child_pid == 0) 
    {
        prctl(PR_SET_PTRACER, getppid(), 0, 0, 0);
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1) 
        {
            perror("ptrace");
            return -1;
        }
        char *args[] = {"/path/to/program", "arg1", "arg2", NULL};
        execv(args[0], args);
        if (errno == ENOENT) 
        {
            printf("File not found\n");
            child_pid = -1;
            return -1;
        }
        perror("execv");
        return -1;
    } 
    else 
    {
        int status;
        waitpid(child_pid, &status, 0);

        if(WIFSTOPPED(status)) 
        {
            printf("Process is already stopped\n");
            return 0;
        }
        printf("Attached to process %d\n", child_pid);
        while (WIFSTOPPED(status)) 
        {
            if (ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL) == -1) 
            {
                perror("ptrace syscall");
                break;
            }
            if (waitpid(child_pid, &status, 0) == -1) 
            {
                perror("waitpid");
                break;
            }
        }
        printf("Detached from process %d\n", child_pid);
        ptrace(PTRACE_DETACH, child_pid, NULL, NULL);
    }

    return 0;
}

int is_process_running(pid_t pid) 
{
    if (kill(pid, 0) == -1) 
	{
        if (errno == ESRCH) 
        {
            printf("Process %d is not running\n", pid);
            return 0;
        }
    }
    printf("Process %d is running\n", pid);
    return 1;
}

void status(pid_t pid) 
{
    if (is_process_running(pid)) 
        printf("Process %d is running\n", pid);
    else 
        printf("Process %d is not running\n", pid);
}