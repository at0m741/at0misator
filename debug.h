#include "stdio.h"
#include "stdlib.h"

debug(){
    pid_t child;
    struct user_regs_struct regs;
    ptrace( PTRACE_GETREGS, child, NULL, &regs );
    printf("rax = 0x%016x\n", regs.rax);
    printf("rbx = 0x%016x\n", regs.rbx );
    printf("rcx = 0x%016x\n", regs.rcx );
    printf("rdx = 0x%016x\n", regs.rcx );
    printf("rbp = 0x%016x\n", regs.rbp );
    printf("rsp = 0x%016x\n", regs.rsp );
    printf("rsi = 0x%016x\n", regs.rsi );
    printf("rdi = 0x%016x\n", regs.rdi );
    printf("r8  = 0x%016x\n", regs.r8 );
    printf("r9  = 0x%016x\n", regs.r9 );
    printf("r10 = 0x%016x\n", regs.r10 );
    printf("r11 = 0x%016x\n", regs.r11 );
    printf("r12 = 0x%016x\n", regs.r12 );
    printf("r13 = 0x%016x\n", regs.r13 );
    printf("r14 = 0x%016x\n", regs.r14 );
    printf("r15 = 0x%016x\n", regs.r15 );


printf("regs status = 0x%016x\n", &regs);

}
