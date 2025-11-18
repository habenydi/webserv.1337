#include "include.hpp"


int	main(int ac, char **av)
{
	(void) av;
	(void) ac;
	Server server;
	int port = 8080; // Later: read from config
	server.run(port);
	return 0;
}

/*
int set_nonblocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1) return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(int ac, char **av) {
	try
	{
		pars var;
		if (ac != 2)
			throw std::runtime_error("Usage: ./webserv <conf file>");
		globale data;
		var.parsing(av[1], data);
		// return (1); // khliha bax ntesty
		const int PORT = 8080;
		int server_fd = socket(AF_INET, SOCK_STREAM, 0);
		if (server_fd == -1) {
			std::cerr << "socket() failed\n";
			return 1;
		}

		// Allow reuse of address
		int opt = 1;
		setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		if (set_nonblocking(server_fd) == -1) {
			std::cerr << "fcntl failed\n";
			close(server_fd);
			return 1;
		}

		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY);
		addr.sin_port = htons(PORT);

		if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
			std::cerr << "bind() failed\n";
			close(server_fd);
			return 1;
		}

		if (listen(server_fd, 10) == -1) {
			std::cerr << "listen() failed\n";
			close(server_fd);
			return 1;
		}

		std::cout << "Server listening on port " << PORT << "\n";

		const char* response = 
			"HTTP/1.0 200 OK\r\n"
			"Content-Type: text/plain\r\n"
			"Content-Length: 18\r\n"
			"\r\n"
			"Hello, webserv!\n";

		struct pollfd fds[100]; // support up to 99 clients + 1 server
		int nfds = 1;
		fds[0].fd = server_fd;
		fds[0].events = POLLIN;

		while (true) {
			int ready = poll(fds, nfds, -1);
			if (ready <= 0) continue;

			// Check for new connection
			if (fds[0].revents & POLLIN) {
				struct sockaddr_in client_addr;
				socklen_t client_len = sizeof(client_addr);
				int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
				if (client_fd != -1) {
					if (set_nonblocking(client_fd) == -1) {
						close(client_fd);
					} else {
						// Add to poll list
						if (nfds < 100) {
							fds[nfds].fd = client_fd;
							fds[nfds].events = POLLOUT; // ready to send response immediately
							nfds++;
						} else {
							close(client_fd); // too many clients
						}
					}
				}
			}

			// Handle clients
			for (int i = 1; i < nfds; ) {
				if (fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
					close(fds[i].fd);
					fds[i] = fds[nfds - 1];
					nfds--;
					continue;
				}

				if (fds[i].revents & POLLOUT) {
					// Send hardcoded response
					send(fds[i].fd, response, strlen(response), 0);
					close(fds[i].fd);
					fds[i] = fds[nfds - 1];
					nfds--;
					continue;
				}
				i++;
			}
		}
		close(server_fd);
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	// try
	// {
	// 	(void)av;
	// 	(void)ac;
	// 	HttpRequest request;
	// 	int fd = open("httpReq", O_RDONLY);
	// 	httpPars http;
	// 	http.RequestPars(fd, request);
	// }
	// catch(const std::exception& e)
	// {
	// 	std::cerr << e.what() << '\n';
	// 	return 1;
	// }
	
	return 0;
}
*/
