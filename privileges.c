#include "debug.h"

void setup_seccomp() 
{
    struct sock_filter filter[] = 
    {
        BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
    };

    struct sock_fprog prog = 
    {
        .len = (unsigned short)(sizeof(filter)/sizeof(filter[0])),
        .filter = filter,
    };

    if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog)) 
    {
        perror("prctl");
        exit(1);
    }
}