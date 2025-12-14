#include "../include.hpp"

void	CGI::setEnv(std::string& file)
{
	(void)file;
	if (chdir(_root.c_str()) == -1)
	{
		std::cerr << "[ERROR] chdir failed" << std::endl;
		return;
	}
	setenv("REQUEST_METHOD", _req.method.c_str(), 1);
	setenv("SCRIPT_NAME", _req.path.c_str(), 1);  // Changed from file.c_str() to _req.path.c_str()
	setenv("QUERY_STRING", _req.query_string.c_str(), 1);
	if (_req.headers.find("Content-Length") != _req.headers.end())
		setenv("CONTENT_LENGTH", _req.headers.find("Content-Length")->second.c_str(), 1);
	if (_req.headers.find("Content-Type") != _req.headers.end())
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
		if (_req.IsPOST) {
			close(ifd[0]);
			close(ifd[1]);
		}
		return ;
	}

	pid_t	pid = fork();
	if (pid == -1)
	{
		std::cerr << "[ERROR] fork fails" << std::endl;
		close(fd[0]);
		close(fd[1]);
		if (_req.IsPOST) {
			close(ifd[0]);
			close(ifd[1]);
		}
		return ;
	}else if (pid == 0)
	{
		setEnv(file);
		if (_req.IsPOST)
		{
			dup2(ifd[0], 0);
			close(ifd[1]);
			close(ifd[0]);
		}
		dup2(fd[1], 1);
		close(fd[1]);
		close(fd[0]);

		char* av[3];
		av[0] = const_cast<char*>(interpreter.c_str());
		av[1] = const_cast<char*>(file.c_str());
		av[2] = NULL;
		execve(interpreter.c_str(), av, env);
		std::exit(1);
	}

	close(fd[1]);

	/////////////////////////////////////
	if (_req.IsPOST) {
		close(ifd[0]); // Close read end of input pipe
		ssize_t written = write(ifd[1], input.c_str(), input.length());
		if (written != (ssize_t)input.length())
		    std::cerr << "[WARNING] incomplete write to CGI stdin" << std::endl;
		close(ifd[1]);
	}
    
	output.clear();
	char buffer[4096];
	ssize_t bytesRead;

	while ((bytesRead = read(fd[0], buffer, sizeof(buffer))) > 0) {
	output.append(buffer, bytesRead);
	}

	close(fd[0]);

	// Wait for child process
	if (waitpid(pid, &status, 0) == -1) {
	std::cerr << "[ERROR] waitpid failed: " << strerror(errno) << std::endl;
	return;
	}

	if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
	std::cerr << "[WARNING] CGI script exited with status " 
		  << WEXITSTATUS(status) << std::endl;
	} else if (WIFSIGNALED(status)) {
	std::cerr << "[ERROR] CGI script terminated by signal " 
		  << WTERMSIG(status) << std::endl;
	}

	std::cout << "\n[DEBUG] CGI Output:\n" << output << std::endl;
}
