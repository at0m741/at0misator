#include "debug.h"

long inspect_variable(pid_t pid, void* addr) 
{
    long data = ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
    if (data == -1) {
        perror("ptrace PEEKDATA");
        return -1;
    }
    return data;
}

void step(pid_t pid) 
{
	if (ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL) == -1) 
	{
		perror("ptrace singlestep");
		return;
	}
	int status;
	if (waitpid(pid, &status, 0) == -1) 
	{
		perror("waitpid");
		return;
	}
	printf("var = %ld\n", inspect_variable(pid, (void*)0x400000));
}

void continue_execution(pid_t pid) 
{
	if (ptrace(PTRACE_CONT, pid, NULL, NULL) == -1) 
	{
		perror("ptrace continue");
		return;
	}
	int status;
	if (waitpid(pid, &status, 0) == -1) 
	{
		perror("waitpid");
		return;
	}
	printf("var = %ld\n", inspect_variable(pid, (void*)0x400000));
}

void breakpoint(pid_t pid, void* addr) 
{
	long data = inspect_variable(pid, addr);
	if (data == -1) 
	{
		return;
	}
	if (ptrace(PTRACE_POKEDATA, pid, addr, (void*)((data & ~0xff) | 0xcc)) == -1) 
	{
		perror("ptrace POKEDATA");
		return;
	}
	printf("breakpoint set at %p\n", addr);
}