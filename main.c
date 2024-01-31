#include "debug.h"

void usage(char *name) 
{
    fprintf(stderr, "Usage: %s [PROGRAMENAME [ARG1 ARG2 ...] | -p PID]\n", name);
}

int main(int argc, char** argv)
{
    if (getuid() != 0) 
    {
        fprintf(stderr, "This program must be run as root.\n");
        return 1;
    }

    printf("=====================================================\n");
    printf("::                                                 ::\n");
    printf("::  at0misator for x86_64, Copyright 2021, at0m741 ::\n");
    printf("::                                                 ::\n");
    printf("::          BUILD_TAG:  at0misator-1.0-1           ::\n");
    printf("::                                                 ::\n");
    printf("::          BUILD_STYLE: DEBUG                     ::\n");
    printf("::                                                 ::\n");
    printf("::          Reversetool                            ::\n");
    printf("::                                                 ::\n");
    printf("=====================================================\n\n");

    if (argc < 2) 
    {
        usage(argv[0]);
        return 1;
    }

    if (argv[1][0] == '-' && argv[1][1] == 'p' && argc > 2) 
    {
        int pid = atoi(argv[2]);
        if (is_process_running(pid)) 
        {
            printf("Process %d is running\n", pid);
            return fork_and_trace(pid);
        } 
        else 
        {
            printf("Process %d is not running\n", pid);
            if (pid > 0) 
            {
                printf("Attaching to PID %d\n", pid);
                setup_seccomp();
                return attach_and_trace(pid);
            }
            else 
            {
                fprintf(stderr, "Invalid PID '%s'\n", argv[2]);
                return 1;
            }
        }
    } 
    else 
    {
        char* program_name = argv[1];
        char** program_argv = &argv[1];
        setup_seccomp();
        pid_t child_pid = fork();
        if (child_pid == 0) 
        {
            execvp(program_name, program_argv);
            perror("execvp");
            exit(1);
        } 
        else if (child_pid < 0) 
        {
            perror("fork");
            return 1;
        } 
        else 
        {
            return attach_and_trace(child_pid);
        }
    }

    return 0;
}