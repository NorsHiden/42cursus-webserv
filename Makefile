CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -I ./inc/ -fsanitize=address -g

SRCS = srcs/main.cpp srcs/tools.cpp srcs/configParse.cpp srcs/initServers.cpp \
		srcs/handleRequest.cpp srcs/setupResponse.cpp srcs/errorRequest.cpp

NAME = webserv

all: $(NAME)

$(NAME): $(SRCS)
	c++ $(CPPFLAGS) $(SRCS) -o $(NAME)

clean:
	rm -f $(NAME)

fclean: clean

re: fclean all