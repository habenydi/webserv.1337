#pragma once

#include "../external.hpp"
#include "../parsing/httpRequest/httpPars.hpp"
#include "../Helper/globale.hpp"

typedef struct Client
{
	struct epoll_event	ev;
	size_t			offset;
	size_t			fd;
	globale			conf;
}	Client;

typedef	struct	sock
{
	int	sockfd;
	int	port;
	globale conf;
}	sock;

class	Server
{
	std::vector<sock>			socks;
	int	epfd;
	httpPars	parsing;

	int	init_first_sock();
	void	epoll_init();
	void	epoll_loop();
	void	accept_client(int sockfd);
	void	_recv(int, globale&);
	void	_send(Client&);
	void	recvRq(Client&);
	void	sendRs(Client&);
	void	closeClient(int fd);
	// ``` the parser object ```
	public:
		void run();
};
