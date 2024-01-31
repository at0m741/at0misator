CC = gcc
NAME = at0misator
SRCS = debugger.c \
	   hexdump.c \
	   main.c \
	   setup.c \
	   run.c \
	   breakpoints.c 

FLAGS = -m64 -no-pie -fno-pie -pthread -fno-asm -g  -llzma -fPIE

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(NAME)  -m64 -no-pie -fno-pie -pthread -fno-asm -g  -llzma

%.o: %.c
	$(CC) -m64 -no-pie -fno-pie -pthread -fno-asm -g -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

debug: $(OBJS)
	$(CC) $(OBJS) -o $(NAME) -m64 -no-pie -fno-pie -pthread -fno-asm -g  -llzma -g

.PHONY: all clean fclean re debug