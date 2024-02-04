#include "debug.h"

void disassemble(uint8_t* code, size_t code_size) 
{
    csh handle;
    cs_insn *insn;
    size_t count;
    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
        return;
    count = cs_disasm(handle, code, code_size, 0x1000, 0, &insn);
    if (count > 0) {
        size_t j;
        for (j = 0; j < count; j++) {
            printf("\033[0;32m0xffffff%"PRIx64"\033[0m: \t%s\t\t%s\t%hhn\n", insn[j].address, insn[j].mnemonic, insn[j].op_str, insn[j].bytes);
        }
        cs_free(insn, count);
    } else {
        printf("ERROR: Failed to disassemble given code!\n");
    }
    cs_close(&handle);
}

void trace_and_disassemble(pid_t pid, void *addr) 
{
    long instruction = ptrace(PTRACE_PEEKTEXT, pid, addr, NULL);
    disassemble((uint8_t*)&instruction, sizeof(instruction) * 10);
    return;
}

/* void get_current_line(pid_t pid, void* addr) 
{
    char* line = get_current_line(pid, addr);
    printf(".c: %s\n", line);
    free(line);
    return;
} */