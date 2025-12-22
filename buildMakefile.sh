#!/bin/bash

# Colors
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

PROJECT_NAME='webserv'
CFLAGS='-Wall -Wextra -Werror -std=c++98'
COMPILER='c++'

echo -e "${GREEN}=== WebServer Makefile Generator ===${NC}"

# Get all .cpp files inside src/, EXCLUDING test files
CPP_FILES=$(find src/ -type f -name "*.cpp" ! -iname "*test*" | sort)

SRCS=$(echo $CPP_FILES | tr '\n' ' ')

echo -e "${GREEN}Generating Makefile with obj/ directory...${NC}"

cat > Makefile << EOF
NAME       = $PROJECT_NAME

CXX        = $COMPILER
CXXFLAGS   = $CFLAGS

SRCS       = $SRCS
OBJDIR     = obj
OBJS       = \$(SRCS:src/%.cpp=\$(OBJDIR)/%.o)

all: \$(NAME)

\$(NAME): \$(OBJDIR) \$(OBJS)
	@\$(CXX) \$(CXXFLAGS) \$(OBJS) -o \$(NAME)
	@echo "\\033[0;32m✓ \$(NAME) compiled successfully!\\033[0m"

\$(OBJDIR):
	@mkdir -p \$(OBJDIR)

\$(OBJDIR)/%.o: src/%.cpp
	@mkdir -p \$(dir \$@)
	@\$(CXX) \$(CXXFLAGS) -c \$< -o \$@

clean:
	@rm -rf \$(OBJDIR)
	@echo "\\033[0;33m✓ Object directory removed\\033[0m"

fclean: clean
	@rm -f \$(NAME)
	@echo "\\033[0;33m✓ Executable removed\\033[0m"

re: fclean all

.PHONY: all clean fclean re
EOF

echo -e "${GREEN}✓ Makefile successfully created!${NC}"
