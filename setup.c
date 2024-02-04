#include "debug.h"

int sigtstp_received = 0;

void sigtstp_handler(int sig) 
{
    printf("Received interrupt %d.\n", sig);
    sigtstp_received = 1;
}

int run_target(const char* programname)
{
    printf("Tracing '%s'.\n", programname);
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) 
	{
        perror("ptrace");
        return 1;
    }
    return execl(programname, programname, 0, NULL);
}

void trace_process(pid_t pid) 
{
    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) 
	{
        perror("ptrace attach");
        return;
    }

    int status;
    if (waitpid(pid, &status, 0) == -1) 
	{
        perror("waitpid");
        return;
    }

    printf("Attached to process %d\n", pid);
    attach_and_trace(pid);
    while (WIFSTOPPED(status)) {
        printf("debug");
        run_debugger(pid);
        if (ptrace(PTRACE_SYSCALL, pid, NULL, NULL) == -1) {
            perror("ptrace syscall");
            break;
        }

        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            break;
        }
    }

    printf("Detached from process %d\n", pid);
    ptrace(PTRACE_DETACH, pid, NULL, NULL);


}

int fork_and_trace(pid_t pid)
{
    if (ptrace(PTRACE_ATTACH, pid, NULL, NULL) == -1) 
	{
        perror("ptrace attach");
        return (1);
    }

    int status;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return (1);
    }

    printf("Attached to process %d\n", pid);
    run_debugger(pid);
    while (WIFSTOPPED(status)) {
        if (ptrace(PTRACE_SYSCALL, pid, NULL, NULL) == -1) 
		{
            perror("ptrace syscall");
            break;
        }

        if (waitpid(pid, &status, 0) == -1) 
		{
            perror("waitpid");
            break;
        }
    }
    printf("Detached from process %d\n", pid);
    ptrace(PTRACE_DETACH, pid, NULL, NULL);
    return (0);
}

