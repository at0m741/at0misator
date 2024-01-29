/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debugger.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouzali <ltouzali@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 16:28:36 by ltouzali          #+#    #+#             */
/*   Updated: 2024/01/29 20:45:47 by ltouzali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"

void debug()
{
    pid_t child;
    struct user_regs_struct regs;
    ptrace( PTRACE_GETREGS, child, NULL, &regs );
    printf("rax = 0x%llx\n", regs.rax);
    printf("rbx = 0x%llx\n", regs.rbx );
    printf("rcx = 0x%llx\n", regs.rcx );
    printf("rdx = 0x%llx\n", regs.rcx );
    printf("rbp = 0x%llx\n", regs.rbp );
    printf("rsp = 0x%llx\n", regs.rsp );
    printf("rsi = 0x%llx\n", regs.rsi );
    printf("rdi = 0x%llx\n", regs.rdi );
    printf("r8  = 0x%llx\n", regs.r8 );
    printf("r9  = 0x%llx\n", regs.r9 );
    printf("r10 = 0x%llx\n", regs.r10 );
    printf("r11 = 0x%llx\n", regs.r11 );
    printf("r12 = 0x%llx\n", regs.r12 );
    printf("r13 = 0x%llx\n", regs.r13 );
    printf("r14 = 0x%llx\n", regs.r14 );
    printf("r15 = 0x%llx\n", regs.r15 );
	printf("rip = 0x%llx\n", regs.rip );
	printf("eflags = 0x%llx\n", regs.eflags );
	printf("cs = 0x%llx\n", regs.cs );
	printf("ss = 0x%llx\n", regs.ss );
	printf("ds = 0x%llx\n", regs.ds );
	printf("es = 0x%llx\n", regs.es );
	printf("fs = 0x%llx\n", regs.fs );
	printf("gs = 0x%llx\n", regs.gs );
}

bool is_symbol(char symbol) 
{
	if (symbol >= 0x20 && symbol <= 0x7e) 
		return true;
	else 
		return false;
}

void print_backtrace(void) 
{
    void *buffer[256];
    int nptrs = backtrace(buffer, 256);
    char **strings = backtrace_symbols(buffer, nptrs);
    if (strings == NULL) 
	{
        perror("backtrace_symbols");
        exit(EXIT_FAILURE);
    }
    for (int j = 0; j < nptrs; j++)
        printf("%s\n", strings[j]);

    free(strings);
}

void trace_memory(pid_t pid, int address, int length) 
{
	unsigned char *data = malloc(length);
	unsigned char *bytes = data;
	int i = 0;
	while (i < length) 
	{
		long word = ptrace(PTRACE_PEEKDATA, pid, address + i, NULL);
		memcpy(bytes, &word, sizeof(word));
		bytes += sizeof(word);
		i += sizeof(word);
	}
	printHex(data, length);
	free(data);
}
