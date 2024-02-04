#include "debug.h"

/**
 This is the main loop of the command prompt using fget (mut replace with readline)
*/

int run_debugger(pid_t child_pid)
{
    int wait_status;
    char command[20];
	long int address = 0x0;
    printf("Starting debugger.\n");
    progname(child_pid);

    while (1) 
	{
		//handle signals
		/* if (signal(SIGTSTP, sigtstp_handler) == SIG_ERR)
			perror("signal");
		if (signal(SIGINT, sigtstp_handler) == SIG_ERR)
			perror("signal");
		if (signal(SIGQUIT, sigtstp_handler) == SIG_ERR)
			perror("signal");
		if (signal(SIGTERM, sigtstp_handler) == SIG_ERR)
			perror("signal");
		if (signal(SIGKILL, sigtstp_handler) == SIG_ERR)
			perror("signal");
		if (signal(SIGSTOP, sigtstp_handler) == SIG_ERR) */
		//start noice goffy debugger prompt
        fputs("\033[0;31mat0misator > \033[0m", stdout);
		fflush(stdout);
		fgets(command, sizeof(command), stdin);

		if (!(strncmp(command, "q", 1))) 
		{
			ptrace(PTRACE_DETACH, child_pid, NULL, NULL);
			printf("Detached. Exiting.\n");
			exit(0);

		} 
		else if (!(strncmp(command, "re", 2))) 
		{
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
		else if (!(strncmp(command, "e", 1))) 
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
			ptrace(SIGTRAP, child_pid, 0, 0);
			printf("Process killed\n");
		} 
		else if (!(strncmp(command, "step", 4)))
		{
			step(child_pid);
			continue;
		}
		else if (!strncmp(command, "hx", 2)) 
		{
			unsigned long addr;
			if (sscanf(command + 2, "%lx", &addr) != 1) {
				printf(" -- error: unable to parse address\n");
				continue;
			}

			printf("Dumping memory from address: %llx\n", (unsigned long long)addr);

			unsigned char buffer[160];
			for (int i = 0; i < 10; i++) 
			{
				errno = 0;
				long word = ptrace(PTRACE_PEEKDATA, child_pid, addr + i * 8, 0);
				if (errno != 0) {
					printf(" -- error: unable to read memory at address: 0x%lx\n", addr + i * 8);
					continue;
				}
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
		else if (!strncmp(command, "d", 1))
		{
			void *addr;
			if (sscanf(command + 1, "%p", &addr) != 1) 
			{
				printf(" -- error: unable to parse address\n");
				continue;
			}
			trace_and_disassemble(child_pid, addr);
			continue;
		}
		else if (!strncmp(command, "c", 1))
		{
			if (ptrace(PTRACE_CONT, child_pid, 0, 0) < 0) 
				perror("ptrace");
			continue_execution(child_pid);
		}
		else if (!strncmp(command, "b", 1))
		{
			if (sscanf(command + 1, "%lx", &address) != 1) 
			{
				printf(" -- error: unable to parse address\n");
				continue;
			}
			breakpoint(child_pid, (void*)address);
			trace_and_disassemble(child_pid, (void*)address);
		}
		else if (!strncmp(command, "l", 1))
		{
			struct user_regs_struct regs;
			if (ptrace(PTRACE_GETREGS, child_pid, 0, &regs) < 0) 
			{
				perror("ptrace(PTRACE_GETREGS)");
				return -1;
			}
			ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
			print_current_line(child_pid, regs.rip);
		}
		else if (!strncmp(command, "info", 4))
		{
			status(child_pid);
			continue;
		}
		else if (!strncmp(command, "attach", 6))
		{
			attach_and_trace(child_pid);
			continue;
		}
		else if (!strncmp(command, "h", 1) || !strncmp(command, "help", 4))
		{
			printf("Your options are:\n");
			printf("  q:  detach and quit\n");
			printf("  i:  step instruction\n");
			printf("  e:  step syscall\n");
			printf("  b:  breakpoint at [addr]\n");
			printf("  k:  kill current process\n");
			printf("  hx: print hexdump [addr], [length]\n");
			printf("  re: print registers value\n");
			printf("  bt: print backtrace\n");
			printf("  mt: print memory trace [addr], [lenght]\n");
			printf("  d:  disassemble [addr]\n");
			printf("  c:  continue execution\n");
			printf("  step:  single step\n");
			printf("  l:  print current line\n");
			printf("  info:  print status info\n");
			printf("  h:  print this help\n");
		}
		else
		{
			printf("Unknown command\n");
			continue;
		}
    }
	free(command);
	return 0;
}