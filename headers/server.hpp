#pragma once

#include <string>
#include "external.hpp"

class	Server
{
	int	_port;
	std::vector<int> _fds;
	int	sockfd;
	int	epfd;

	int	init_first_sock(int port);
	void	epoll_loop();
	void	accept_client();
	void	_recv(int);
	void	_send(int);
	// ``` parser object ```
	public:
		
		void run(int port);
};
