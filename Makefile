CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -I ./inc/

SRCS = srcs/main.cpp srcs/tools.cpp srcs/configParse.cpp

NAME = webserv

all: $(NAME)

$(NAME): $(SRCS)
	c++ $(CPPFLAGS) $(SRCS) -o $(NAME)

clean:
	rm -f $(NAME)

fclean: clean

re: fclean all