#include "debug.h"

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
    printf("ELF version: %d\n", ehdr.e_ident[EI_VERSION]);
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
    printf("ELF program header table:\n");

    close(fd);
}