#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

CFLAGS='-Wall -Wextra -Werror -std=c++98'
COMPILER='c++'
PROJECT_NAME='webserv'

echo -e "${GREEN}=== C++ Makefile Generator ===${NC}\n"

# Get project name
##read -p "Enter project name: " PROJECT_NAME
##if [ -z "$PROJECT_NAME" ]; then
##    echo -e "${RED}Error: Project name cannot be empty${NC}"
##    exit 1
##fi

# Get compiler (default: c++)
##read -p "Enter compiler (default: c++): " COMPILER
##COMPILER=${COMPILER:-c++}

# Get compilation flags (default: -Wall -Wextra -Werror -std=c++98)
##read -p "Enter compilation flags (default: -Wall -Wextra -Werror -std=c++98): " CFLAGS
##CFLAGS= "-Wall -Wextra -Werror -std=c++98" #${CFLAGS:--Wall -Wextra -Werror -std=c++98}

# Find all .cpp files in current directory
CPP_FILES=$(find ./src/ -maxdepth 1 -name "*.cpp" -type f | sort)

if [ -z "$CPP_FILES" ]; then
    echo -e "${YELLOW}Warning: No .cpp files found in current directory${NC}"
    read -p "Enter source files manually (space-separated): " CPP_FILES
fi

# Convert to space-separated list
SRCS=$(echo $CPP_FILES | tr '\n' ' ')

echo -e "\n${GREEN}Generating Makefile...${NC}\n"

# Create the Makefile
cat > Makefile << EOF
NAME	 = $PROJECT_NAME

CXX	 = $COMPILER
CXXFLAGS = $CFLAGS

SRCS	 = $SRCS

OBJS	 = \$(SRCS:.cpp=.o)

all: \$(NAME)

\$(NAME): \$(OBJS)
	\$(CXX) \$(CXXFLAGS) \$(OBJS) -o \$(NAME)
	@echo "\033[0;32m✓ \$(NAME) compiled successfully!\033[0m"

%.o: %.cpp
	\$(CXX) \$(CXXFLAGS) -c \$< -o \$@

clean:
	@rm -f \$(OBJS)
	@echo "\033[0;33m✓ Object files cleaned\033[0m"

fclean: clean
	@rm -f \$(NAME)
	@echo "\033[0;33m✓ Executable removed\033[0m"

re: fclean all

.PHONY: all clean fclean re
EOF

echo -e "${GREEN}✓ Makefile created successfully!${NC}\n"

# Display summary
echo -e "${YELLOW}Summary:${NC}"
echo "  Project Name: $PROJECT_NAME"
echo "  Compiler: $COMPILER"
echo "  Flags: $CFLAGS"
echo "  Source Files:"
for src in $SRCS; do
    echo "    - $src"
done

echo -e "\n${GREEN}Available rules:${NC}"
echo "  make all     - Compile the project"
echo "  make clean   - Remove object files"
echo "  make fclean  - Remove object files and executable"
echo "  make re      - Recompile everything from scratch"

echo -e "\n${GREEN}Done! You can now run 'make' to compile your project.${NC}"
