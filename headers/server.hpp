#pragma once

#include <string>
#include <sys/types.h>
#include "external.hpp"

typedef struct Client
{
	struct epoll_event	ev;
	size_t			offset;
	size_t			fd;
}	Client;

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
	void	_send(Client&);
	void	recvRq(struct epoll_event&, int fd);
	void	sendRs(Client&);
	void	closeClient(int fd);
	// ``` the parser object ```
	public:
		
		void run(int port);
};
