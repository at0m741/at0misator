CC = gcc
NAME = at0misator
SRCS = debugger.c \
	   hexdump.c \
	   main.c \
	   setup.c \
	   run.c \
	   breakpoints.c \
	   trace.c \
	   elf.c \
	   disassembler.c \
	   privileges.c 

FLAGS = -m64 -no-pie -fno-pie -pthread -fno-asm -g  -lunwind -lunwind-ptrace -llzma

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME) -Werror -Wextra -Wall -m64 -no-pie -fno-pie -pthread -fno-asm -lunwind-ptrace -lunwind-generic -lunwind -lreadline -lcapstone -ldwarf -lelf -lz -llzma -lcapstone -lreadline -ldw


%.o: %.c
	$(CC) -Werror -Wextra -Wall -m64 -no-pie -fno-pie -pthread -fno-asm -g -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

debug: $(OBJS)
	$(CC) $(OBJS) -o $(NAME) -m64 -no-pie -fno-pie -pthread -fno-asm -g -lunwind-ptrace -lunwind-generic -lunwind -lreadline -lcapstone -ldwarf -ldw


.PHONY: all clean fclean re debug