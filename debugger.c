#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/ptrace.h>
#include <sys/reg.h> /* EAX, ORIG_EAX */
#include <sys/user.h>
#include "stdbool.h"
#include "debug.h"
#include <sys/syscall.h>


#define MAX_SYSCALL_NUM 337
int sigtstp_received = 0;
void sigtstp_handler(int sig) {
    printf("Received interrupt %d.\n", sig);
    sigtstp_received = 1;
}


int run_target(const char* programname)
{
    printf("Tracing '%s'.\n", programname);
    if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
        perror("ptrace");
        return 1;
    }
    return execl(programname, programname, 0, NULL);
}
bool is_symbol(char symbol) {
	if (symbol >= 0x20 && symbol <= 0x7e) 
		return true;
	else 
		return false;
}

int run_debugger(pid_t child_pid)
{
    int wait_status;
    char command[20];
	int address;
	int length;
    printf("Starting debugger.\n");

    
    wait(&wait_status);

    while (1) {
        fputs("at0misator > ", stdout);
	fflush(stdout);
	fgets(command, sizeof(command), stdin);

	if (!(strncmp(command, "q", 1))) {
	    ptrace(PTRACE_DETACH, child_pid, NULL, NULL);
	    return 0;
	} else if (!(strncmp(command, "re", 2))) {
	    (void) signal(SIGTSTP, sigtstp_handler);
        int pid;
		int regs;
		debug();
    	
		return 0;
 
	} else if (!strncmp(command, "cs", 2)) {
	    (void) signal(SIGTSTP, sigtstp_handler);
	} else if (!(strncmp(command, "i", 1))) {
	    if (ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0) < 0) {
	        perror("ptrace");
		return 1;
	    }
	    wait(&wait_status);

	    struct user_regs_struct regs;
	    ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
	    long instr = ptrace(PTRACE_PEEKTEXT, child_pid, regs.rip, 0);
 	    printf("EIP = %08lx, instr = %08lx\n", regs.rip, instr);
	} else if (!(strncmp(command, "s", 1))) {
	    if (ptrace(PTRACE_SYSCALL, child_pid, 0, 0) < 0) {
  	        perror("ptrace");
		return 1;
	    }
	    wait(&wait_status);
	    int syscall = ptrace(PTRACE_PEEKUSER, child_pid, sizeof(long)*ORIG_RAX);

	    if (ptrace(PTRACE_SYSCALL, child_pid, 0, 0) < 0) {
  	        perror("ptrace");
		return 1;
	    }
	    wait(&wait_status);
	    int retval = ptrace(PTRACE_PEEKUSER, child_pid, sizeof(long)*RAX);

	    printf("syscall(%d) = %d\n", syscall, retval);
	} else if (!(strncmp(command, "k", 1))) {
		kill(child_pid, SIGSTOP);
	
	} else {
	  printf("Your options are:\n");
	  printf("  q: detach and quit\n");
	  printf("  i: step instruction\n");
	  printf("  s: step syscall\n");
	  printf("  k: kill current process\n");
	  printf("  hx: print hexdump [addr], [length]\n");
      printf("  re: print registers value\n");

	}

	if (WIFEXITED(wait_status)) {
	    printf("Done!\n");
	    return 0;
	}
    }
}


int fork_and_trace(char *programname) {
    pid_t child_pid = fork();
    if (child_pid == 0)
        return run_target(programname);
    else if (child_pid > 0)
        return run_debugger(child_pid);
    else {
        perror("fork");
        return 1;
    }
}


int attach_and_trace(int pid) {
    ptrace(PTRACE_ATTACH, pid, NULL, NULL);
    return run_debugger(pid);
	if(pid = NULL){
		printf("nop");
	}
}


void usage(char *name) {
    fprintf(stderr, "Usage: %s [PROGRAMENAME | -p PID]\n", name);
}


int main(int argc, char** argv)
{
    printf("========================================\n");
	printf("::\n");
	printf(":: at0misator for x86_64, Copyright 2021, at0m741\n");
	printf("::\n");
	printf("::       BUILD_TAG:  at0misator-1.0-1\n");
	printf("::\n");
	printf("::       BUILD_STYLE: DEBUG\n");
	printf("::\n");
	printf("::       reversetool\n");
	printf("::\n");
	printf("========================================\n\n");

    if (argc < 2) {
        usage(argv[0]);
	return 1;
    }

    if(argv[1][0] == '-') {
        switch(argv[1][1]) {
        case 'h':
	    usage(argv[0]);
	    return 1;
	case 'p':
	    return attach_and_trace(atoi(argv[2]));
	default:
	    usage(argv[0]);
	    return 1;
	}
    } else {
        return fork_and_trace(argv[1]);
    }

    return 0;
}