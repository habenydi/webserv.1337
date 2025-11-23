#include "../include.hpp"
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>

CGI::CGI(int	script)
{
	if (script)
		python();
	else
		javaScript();
}

void	CGI::python()
{
	runScript("/usr/bin/python3", "./www/hello.py");
}

void	CGI::javaScript()
{
	runScript("/usr/bin/nodejs", "./www/script.js");
}

void	CGI::runScript(std::string enterpreter, std::string args)
{
	char	**av = NULL;
	int	fd[2];
	int	status;

	av[0] = strdup(enterpreter.c_str());
	av[1] = strdup(args.c_str());

	if (pipe(fd) == -1)
	{
		std::cerr << "[ERROR] fork fails" << std::endl;
		return;
	}
	pid_t	pid = fork();
	if (pid == -1)
	{
		std::cerr << "[ERROR] fork fails" << std::endl;
		return ;
	} else if (pid == 0)
	{
		dup2(fd[1], 1);
		close(fd[1]);
		close(fd[0]);
		execve(enterpreter.c_str(), av, env);
		std::exit(1);
	}
	waitpid(0, &status, 0);
	close(fd[1]);

	char	line[1000];
	do {
		read(fd[0], line, 1000);
		output += line;
	} while(std::string(line).length() == 1000);
	close(fd[0]);
	std::cout << "\n [DEBUG] \n\n" << output << std::endl;
}
