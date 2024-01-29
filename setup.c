#include "debug.h"

int sigtstp_received = 0;

void sigtstp_handler(int sig) 
{
    printf("Received interrupt %d.\n", sig);
    sigtstp_received = 1;
}

/* void set_capabilities() {
    cap_t caps = cap_init();
    cap_value_t cap_list[] = {CAP_SYS_PTRACE, CAP_SYS_ADMIN};  // Adjust as needed
    cap_set_flag(caps, CAP_EFFECTIVE, sizeof(cap_list) / sizeof(cap_value_t), cap_list);
    cap_set_flag(caps, CAP_PERMITTED, sizeof(cap_list) / sizeof(cap_value_t), cap_list);
    cap_set_flag(caps, CAP_INHERITABLE, sizeof(cap_list) / sizeof(cap_value_t), cap_list);
    cap_set_proc(caps);
    cap_free(caps);
} */

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

    while (WIFSTOPPED(status)) {
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
}

void setup_seccomp() 
{
    prctl(PR_SET_SECCOMP, SECCOMP_MODE_STRICT);
}


int attach_and_trace(pid_t pid) 
{
    pid_t child_pid = fork();

    if (child_pid == -1) 
	{
        perror("fork");
        return -1;
    }
    if (child_pid == 0) 
	{
		/* cap_t caps = cap_init();
		cap_set_flag(caps, CAP_EFFECTIVE, 1, (cap_value_t[]){CAP_SYS_ADMIN});
		cap_set_flag(caps, CAP_PERMITTED, 1, (cap_value_t[]){CAP_SYS_ADMIN});
		cap_set_flag(caps, CAP_INHERITABLE, 1, (cap_value_t[]){CAP_SYS_ADMIN});
		cap_set_proc(caps);
		cap_free(caps); */
        prctl(PR_SET_PTRACER, getppid(), 0, 0, 0);
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1) 
		{
            perror("ptrace");
            return -1;
        }

        char *args[] = {"/path/to/program", "arg1", "arg2", NULL};
        execv(args[0], args);
        perror("execv");
        return -1;
    } 
	else 
	{
        int status;
        waitpid(child_pid, &status, 0);
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
            return 0;
    }
    return 1;
}