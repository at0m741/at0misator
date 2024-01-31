#include "debug.h"

int run_debugger(pid_t child_pid)
{
    int wait_status;
    char command[20];
	int address;
	int length;
    printf("Starting debugger.\n");
    progname(child_pid);
    /* wait(&wait_status);
	prctl(PR_SET_KEEPCAPS, 1); */

    while (1) 
	{
        fputs("at0misator > ", stdout);
		fflush(stdout);
		fgets(command, sizeof(command), stdin);

		if (!(strncmp(command, "q", 1))) 
		{
			ptrace(PTRACE_DETACH, child_pid, NULL, NULL);
			return 0;
		} else if (!(strncmp(command, "re", 2))) 
		{
			/* (void) signal(SIGTSTP, sigtstp_handler); */
			debug(child_pid); 
		} 
		else if (!strncmp(command, "cs", 2)) 
		{
			(void) signal(SIGTSTP, sigtstp_handler);
		} 
		else if (!(strncmp(command, "i", 1))) 
		{
			if (ptrace(PTRACE_SINGLESTEP, child_pid, 0, 0) < 0) 
				perror("ptrace");
			wait(&wait_status);

			struct user_regs_struct regs;
			ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
			long instr = ptrace(PTRACE_PEEKTEXT, child_pid, regs.rip, 0);
			printf("EIP = %08llx, instr = %08lx\n", regs.rip, instr);
		} 
		else if (!(strncmp(command, "s", 1))) 
		{
			if (ptrace(PTRACE_SYSCALL, child_pid, 0, 0) < 0) 
				perror("ptrace");
			wait(&wait_status);
			int syscall = ptrace(PTRACE_PEEKUSER, child_pid, sizeof(long)*ORIG_RAX);

			if (ptrace(PTRACE_SYSCALL, child_pid, 0, 0) < 0) 
				perror("ptrace");
			wait(&wait_status);
			int retval = ptrace(PTRACE_PEEKUSER, child_pid, sizeof(long)*RAX);
			printf("syscall(%d) = %d\n", syscall, retval);
			trace_symbols(child_pid);
		} 
		else if (!(strncmp(command, "k", 1))) 
		{
			kill(child_pid, SIGSTOP);
			printf("Process killed\n");
		} 
		else if (!strncmp(command, "hx", 2)) 
		{
			unsigned long addr;
			sscanf(command + 2, "%lx", &addr);

			unsigned char buffer[160];
			for (int i = 0; i < 10; i++) 
			{
				long word = ptrace(PTRACE_PEEKDATA, child_pid, addr + i * 8, 0);
				memcpy(buffer + i * 8, &word, 8);
			}

			printHex(buffer, sizeof(buffer));
		}
		else if (!(strncmp(command, "bt", 2)))
		{
			printf("Backtrace:\n");
			print_backtrace(child_pid);
		}
		else if (!(strncmp(command, "mt", 2)))
		{
			printf("Memory trace:\n");
			trace_memory(child_pid, address + 10, 100);
		}
		else 
		{
			printf("Your options are:\n");
			printf("  q: detach and quit\n");
			printf("  i: step instruction\n");
			printf("  s: step syscall\n");
			printf("  k: kill current process\n");
			printf("  hx: print hexdump [addr], [length]\n");
			printf("  re: print registers value\n");
			printf("  bt: print backtrace\n");
			printf("  mt: print memory trace [addr], [lenght]\n");
		}
    }
}