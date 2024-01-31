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

void debug(pid_t child)
{
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

void print_backtrace(pid_t child_pid) {
    struct user_regs_struct regs;
    if (ptrace(PTRACE_GETREGS, child_pid, NULL, &regs) < 0) {
        perror("ptrace(PTRACE_GETREGS)");
        return;
    }

    printf("Child process stopped at IP = 0x%llx\n", regs.rip);

    // Print the stack trace.
    for (int i = 0; i < 16; i++) {
        long stack_data = ptrace(PTRACE_PEEKDATA, child_pid, regs.rsp + i * 8, NULL);
        if (stack_data == -1 && errno != 0) {
            perror("ptrace(PTRACE_PEEKDATA)");
            break;
        }
        printf("Stack[%02d] = 0x%lx\n", i, stack_data);
    }
}

void trace_memory(pid_t pid, unsigned long address, int length) 
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

    printf("Memory dump at address 0x%lx:\n", address);
    printHex(data, length);
    free(data);

    // Print the current memory used by the child process.
    char statm_path[256];
    snprintf(statm_path, sizeof(statm_path), "/proc/%d/statm", pid);
    FILE *statm_file = fopen(statm_path, "r");
    if (statm_file == NULL) {
        perror("fopen");
        return;
    }
    unsigned long size, resident, share, text, lib, data1, dt;
    if (fscanf(statm_file, "%lu %lu %lu %lu %lu %lu %lu",
               &size, &resident, &share, &text, &lib, &data1, &dt) != 7) {
        perror("fscanf");
        fclose(statm_file);
        return;
    }
    fclose(statm_file);
    printf("Current memory used by child process: %lu pages\n", resident);
}

void progname(pid_t pid)
{
    char path[1024];
    char *name;
    sprintf(path, "/proc/%d/exe", pid);
    name = malloc(1024);
    readlink(path, name, 1024);
    printf("traced program name: %s\n", name);
    free(name);
}

#include <elf.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

void trace_symbols(pid_t child) {
    char path[1024];
    sprintf(path, "/proc/%d/exe", child);

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }

    Elf64_Ehdr ehdr;
    if (read(fd, &ehdr, sizeof(ehdr)) != sizeof(ehdr)) {
        perror("read");
        close(fd);
        return;
    }

    printf("ELF magic: %02x %02x %02x %02x\n", ehdr.e_ident[0], ehdr.e_ident[1], ehdr.e_ident[2], ehdr.e_ident[3]);
    printf("ELF class: %s\n", ehdr.e_ident[EI_CLASS] == ELFCLASS32 ? "32-bit" : "64-bit");
    printf("ELF data: %s\n", ehdr.e_ident[EI_DATA] == ELFDATA2LSB ? "little-endian" : "big-endian");
    printf("ELF version: %d\n", ehdr.e_ident[EI_VERSION]);
    printf("ELF OS/ABI: %d\n", ehdr.e_ident[EI_OSABI]);
    printf("ELF ABI version: %d\n", ehdr.e_ident[EI_ABIVERSION]);
    printf("ELF type: %d\n", ehdr.e_type);
    printf("ELF machine: %d\n", ehdr.e_machine);
    printf("ELF version: %d\n", ehdr.e_version);
    printf("ELF entry point: 0x%llx\n", (unsigned long long)ehdr.e_entry);
    printf("ELF program header offset: 0x%llx\n", (unsigned long long)ehdr.e_phoff);
    printf("ELF section header offset: 0x%llx\n", (unsigned long long)ehdr.e_shoff);
    printf("ELF flags: 0x%x\n", ehdr.e_flags);
    printf("ELF header size: %d\n", ehdr.e_ehsize);
    printf("ELF program header entry size: %d\n", ehdr.e_phentsize);
    printf("ELF program header entry count: %d\n", ehdr.e_phnum);
    printf("ELF section header entry size: %d\n", ehdr.e_shentsize);
    printf("ELF section header entry count: %d\n", ehdr.e_shnum);
    printf("ELF section header string table index: %d\n", ehdr.e_shstrndx);

    close(fd);
}