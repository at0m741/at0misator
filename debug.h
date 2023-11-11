/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: at0m <at0m@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 16:28:41 by ltouzali          #+#    #+#             */
/*   Updated: 2023/11/11 14:05:35 by at0m             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# ifndef DEBUG_H
#define DEBUG_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/reg.h> /* EAX, ORIG_EAX */
#include <sys/user.h>
#include "stdbool.h"
#include <sys/syscall.h>
#include <errno.h>

void debug();
void	printHex(unsigned char *data, size_t size);
int		run_target(const char* programname);
bool	is_symbol(char symbol);
int		run_debugger(pid_t child_pid);
int		fork_and_trace(char *programname);
int		attach_and_trace(int pid);
void	usage(char *name);

#endif