#include "../include.hpp"

void	CGI::setEnv(std::string& file)
{
	chdir(_root.c_str());
	setenv("REQUEST_METHOD", _req.method.c_str(), 1);
	setenv("SCRIPT_NAME", file.c_str(), 1);
	setenv("QUERY_STRING", _req.query_string.c_str(), 1);
	setenv("CONTENT_LENGTH", _req.headers.find("Content-Length")->second.c_str(), 1);
	setenv("CONTENT_TYPE", _req.headers.find("Content-Type")->second.c_str(), 1);
	setenv("SERVER_PROTOCOL", "HTTP/1.0", 1);
	setenv("GATEWAY_INTERFACE", "CGI/1.1", 1);
	setenv("SERVER_NAME", "localhost", 1);
	setenv("SERVER_PORT", "8080", 1);
	setenv("REMOTE_ADDR", "127.0.0.1", 1);
}

CGI::CGI(std::string root, HttpRequest req) : _req(req), _root(root) {}

void	CGI::run(std::string interpreter, std::string file, std::string input)
{
	char	**av = NULL;
	int	fd[2];
	int	ifd[2];
	int	status;

	
	if (_req.IsPOST)
		if (pipe(ifd) == -1)
		{
			std::cerr << "[ERROR] pipe fails" << std::endl;
			return ;
		}
	if (pipe(fd) == -1)
	{
		std::cerr << "[ERROR] pipe fails" << std::endl;
		return ;
	}

	av[0] = strdup(interpreter.c_str());
	av[1] = strdup(file.c_str());
	av[2] = strdup(NULL);

	pid_t	pid = fork();
	if (pid == -1)
	{
		std::cerr << "[ERROR] fork fails" << std::endl;
		return ;
	}else if (pid == 0)
	{
		setEnv(file);
		if (_req.IsPOST)
		{
			dup2(ifd[0], 0);
			close(ifd[1]);
			close(ifd[0]);
			write(0, input.c_str(), input.length());
		}
		dup2(fd[1], 1);
		close(fd[1]);
		close(fd[0]);
		execve(interpreter.c_str(), av, env);
		std::exit(1);
	}

	wait(&status);
	close(fd[1]);
	if (_req.IsPOST)
	{
		close(ifd[1]);
		close(ifd[0]);
	}

	char	line[1000];
	while (read(fd[0], line, 1000) != 0)
		output += line;
		
	close(fd[0]);
	std::cout << "\n [DEBUG] \n\n" << output << std::endl;

}
