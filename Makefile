NAME       = webserv

CXX        = c++
CXXFLAGS   = -Wall -Wextra -Werror -std=c++98

SRCS       = src/CGI/cgi.cpp src/Helper/CreatAndWrite.cpp src/main.cpp src/parsing/conf_file/pars.cpp src/parsing/httpRequest/httpPars.cpp src/parsing/httpResponse/response.cpp src/server/epoll_loop.cpp src/server/recv_send.cpp src/server/server.cpp 
OBJDIR     = obj
OBJS       = $(SRCS:src/%.cpp=$(OBJDIR)/%.o)

all: $(NAME)

$(NAME): $(OBJDIR) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "\033[0;32m✓ $(NAME) compiled successfully!\033[0m"

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJDIR)
	@echo "\033[0;33m✓ Object directory removed\033[0m"

fclean: clean
	@rm -f $(NAME)
	@echo "\033[0;33m✓ Executable removed\033[0m"

re: fclean all

.PHONY: all clean fclean re
