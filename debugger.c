/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debugger.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouzali <ltouzali@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 16:28:36 by ltouzali          #+#    #+#             */
/*   Updated: 2023/11/10 17:32:39 by ltouzali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"

#define MAX_SYSCALL_NUM 337
int sigtstp_received = 0;
void sigtstp_handler(int sig) {
    printf("Received interrupt %d.\n", sig);
    sigtstp_received = 1;
}

void debug()
{
    pid_t child;
    struct user_regs_struct regs;
    ptrace( PTRACE_GETREGS, child, NULL, &regs );
    printf("rax = 0x%016llx\n", regs.rax);
    printf("rbx = 0x%016llx\n", regs.rbx );
    printf("rcx = 0x%016llx\n", regs.rcx );
    printf("rdx = 0x%016llx\n", regs.rcx );
    printf("rbp = 0x%016llx\n", regs.rbp );
    printf("rsp = 0x%016llx\n", regs.rsp );
    printf("rsi = 0x%016llx\n", regs.rsi );
    printf("rdi = 0x%016llx\n", regs.rdi );
    printf("r8  = 0x%016llx\n", regs.r8 );
    printf("r9  = 0x%016llx\n", regs.r9 );
    printf("r10 = 0x%016llx\n", regs.r10 );
    printf("r11 = 0x%016llx\n", regs.r11 );
    printf("r12 = 0x%016llx\n", regs.r12 );
    printf("r13 = 0x%016llx\n", regs.r13 );
    printf("r14 = 0x%016llx\n", regs.r14 );
    printf("r15 = 0x%016llx\n", regs.r15 );


	printf("regs status = 0x%016llx\n", &regs);

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
    	
		//return 0;
 
	} else if (!strncmp(command, "cs", 2)) {
	    (void) signal(SIGTSTP, sigtstp_handler);
	} else if (!(strncmp(command, "i", 1))) {
	    if (ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0) < 0) {
	        perror("ptrace");
		//return 1;
	    }
	    wait(&wait_status);

	    struct user_regs_struct regs;
	    ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
	    long instr = ptrace(PTRACE_PEEKTEXT, child_pid, regs.rip, 0);
 	    printf("EIP = %08llx, instr = %08llx\n", regs.rip, instr);
	} else if (!(strncmp(command, "s", 1))) {
	    if (ptrace(PTRACE_SYSCALL, child_pid, 0, 0) < 0) {
  	        perror("ptrace");
	    }
	    wait(&wait_status);
	    int syscall = ptrace(PTRACE_PEEKUSER, child_pid, sizeof(long)*ORIG_RAX);

	    if (ptrace(PTRACE_SYSCALL, child_pid, 0, 0) < 0) {
  	        perror("ptrace");
	    }
	    wait(&wait_status);
	    int retval = ptrace(PTRACE_PEEKUSER, child_pid, sizeof(long)*RAX);
	    printf("syscall(%d) = %d\n", syscall, retval);
	} else if (!(strncmp(command, "k", 1))) {
		kill(child_pid, SIGSTOP);
	
	} else if  (!(strncmp(command, "hx", 1))) 
	{
	    if (ptrace(PTRACE_SYSCALL, child_pid, 0, 0) < 0) 
		{
  	        perror("ptrace");
		}
		FILE *fptr;
    	fptr = fopen(argv[1], "rb");

    	if (fptr == NULL)
    	{
    	    printf("error");
    	    return 1;
    	}

   		fseek(fptr, 0, SEEK_END);
    	unsigned long filesize = ftell(fptr);
    	fseek(fptr, 0, SEEK_SET);
    
    	unsigned char *buffer = malloc(filesize);
    	if (buffer == NULL)
   		{
        	printf("Malloc error");
        	return 1;
    	}
    	fread(buffer, 1, filesize, fptr);
    	fclose(fptr);

    	size_t dumpsize = atoi(argv[2]);
    	if (dumpsize > filesize)
    	{
        	dumpsize = filesize;
    	}
    

    	printHex(buffer, dumpsize);

    free(buffer);
    return 0;    
	}else {
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
	    //return 0;
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
	if(pid == 0){
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