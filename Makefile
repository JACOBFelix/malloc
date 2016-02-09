CC	=	gcc

RM	=	rm -f

NAME	=	libmy_malloc_$(HOSTTYPE).so

CFLAGS	+=	-Wall -Werror -Wextra -W -I ./includes/ -fPIC -shared -lpthread

DSRC	=	./srcs/

SRC	=	$(DSRC)malloc.c \
		$(DSRC)realloc.c \
		$(DSRC)calloc.c \
		$(DSRC)free.c \
		$(DSRC)bonus_fonction.c \
		$(DSRC)shared_fonction.c \
		$(DSRC)hook_fonction.c \
		$(DSRC)sbrk_caller.c

OBJS	=	$(SRC:.c=.o)

all:	$(NAME) 

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	ln -f $(NAME) libmy_malloc.so
clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)
	$(RM) libmy_malloc.so

re: fclean all

.PHONY: all clean fclean re
