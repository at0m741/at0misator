/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ltouzali <ltouzali@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 16:28:41 by ltouzali          #+#    #+#             */
/*   Updated: 2024/01/29 20:46:01 by ltouzali         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef DEBUG_H
# define DEBUG_H
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <time.h>
# include <sys/ptrace.h>
# include <sys/types.h>
# include <sys/reg.h> /* EAX, ORIG_EAX */
# include <sys/user.h>
# include "stdbool.h"
# include <sys/syscall.h>
# include <errno.h>
# include <sys/wait.h>
# include <unistd.h>
# include <execinfo.h>
# include <sys/prctl.h>
# include <linux/seccomp.h>
# include <sys/prctl.h>

//# include <sys/capability.h>

#define MAX_SYSCALL_NUM 337

void	sigtstp_handler(int sig);
void	trace_process(pid_t pid);

void 	debug();
void	printHex(unsigned char *data, size_t size);
int		run_target(const char* programname);
bool	is_symbol(char symbol);
int		run_debugger(pid_t child_pid);
int		fork_and_trace(pid_t pid);
int 	attach_and_trace(pid_t pid);
void	usage(char *name);
void 	setup_seccomp();
int 	is_process_running(pid_t pid);
int 	run_debugger(pid_t child_pid);
void 	print_backtrace(void);
bool 	is_symbol(char symbol);
void 	trace_memory(pid_t pid, int address, int length);



#endif