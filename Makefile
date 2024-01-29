CC = gcc
NAME = at0misator
SRCS = debugger.c \
	   hexdump.c \
	   main.c \
	   setup.c \
	   run.c

FLAGS = -m64 -no-pie -fno-pie -pthread -fno-asm -g

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

debug: $(OBJS)
	$(CC) $(FLAGS) $(OBJS) -o $(NAME) -g

.PHONY: all clean fclean re debug