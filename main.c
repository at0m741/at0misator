#include "debug.h"

void usage(char *name) 
{
    fprintf(stderr, "Usage: %s [PROGRAMENAME | -p PID]\n", name);
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

	if (argc < 3) 
	{
        usage(argv[0]);
        return 1;
    }

    int pid = atoi(argv[2]);
    pid_t pid_trace = pid;

    if (argv[1][0] == '-') 
	{
        if (argv[1][1] == 'p' && argc > 2) 
		{
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
            usage(argv[0]);
            return 1;
        }
    } 
	else 
	{
        setup_seccomp();
        return fork_and_trace(pid);
    }

    return 0;
}