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
    if (ptrace(PTRACE_SINGLESTEP, pid, 0, 0) < 0) {
        perror("ptrace(PTRACE_SINGLESTEP)");
        return;
    }
	//int wait_status;
    //wait(&wait_status);
    struct user_regs_struct regs;
    if (ptrace(PTRACE_GETREGS, pid, 0, &regs) < 0) {
        perror("ptrace(PTRACE_GETREGS)");
        return;
    }
    printf("RIP = %08llx\n", regs.rip);
	printf("RAX = %08llx\n", regs.rax);
	printf("RBX = %08llx\n", regs.rbx);
	trace_and_disassemble(pid, (void*)regs.rip);
	print_current_line(pid, regs.rip);
	return;
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
	trace_and_disassemble(pid, addr);
}

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


void print_current_line(pid_t child_pid, Dwarf_Addr addr) 
{
    ptrace(PTRACE_ATTACH, child_pid, NULL, NULL);
    waitpid(child_pid, NULL, 0);
    char filename[256];
    sprintf(filename, "/proc/%d/exe", child_pid);
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
	{
        perror("open");
        return;
    }
    Dwarf *dw = dwarf_begin(fd, DWARF_C_READ);
    if (!dw) 
	{
        fprintf(stderr, "dwarf_begin() failed\n");
        close(fd);
        return;
    }
    Dwarf_Die die;
    if (!dwarf_offdie(dw, 0, &die)) 
	{
        fprintf(stderr, "dwarf_offdie() failed\n");
        return;
    }
    Dwarf_Addr base = 0;
    Dwarf_Line *line = dwarf_getsrc_die(&die, addr - base);
    if (line) 
	{
        int lineno;
        dwarf_lineno(line, &lineno);
        printf("Current line: %d\n", lineno);
    } 
	else 
        printf("No line information for address 0x%lx\n", addr);
	printf("var = %ld\n", inspect_variable(child_pid, (void*)0x400000));
    dwarf_end(dw);
    close(fd);
}	
