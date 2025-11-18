
NAME	 = webserv
CXX	 = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

SRCS	 = ./src/epoll_loop.cpp ./src/main.cpp ./src/pars.cpp ./src/recv_send.cpp ./src/server.cpp 

OBJS	 = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "\033[0;32m✓ $(NAME) compiled successfully!\033[0m"

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)
	@echo "\033[0;33m✓ Object files cleaned\033[0m"

fclean: clean
	@rm -f $(NAME)
	@echo "\033[0;33m✓ Executable removed\033[0m"

re: fclean all

.PHONY: all clean fclean re
