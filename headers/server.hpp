#pragma once

#include "external.hpp"
#include "httpPars.hpp"
#include "globale.hpp"

typedef struct Client
{
	struct epoll_event	ev;
	size_t			offset;
	size_t			fd;
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
	//std::map<int, globale&>	confs;
	int	epfd;
	httpPars	parsing;

	int	init_first_sock();
	void	epoll_init();
	void	epoll_loop();
	void	accept_client(int sockfd);
	void	_recv(int);
	void	_send(Client&);
	void	recvRq(struct epoll_event&, int fd);
	void	sendRs(Client&);
	void	closeClient(int fd);
	// ``` the parser object ```
	public:
		
	void run();
};
