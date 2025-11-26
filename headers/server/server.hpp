#pragma once

#include "../external.hpp"
#include "../parsing/httpRequest/httpPars.hpp"
#include "../parsing/conf_file/globale.hpp"
#include <csignal>

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
	std::vector<sock>	socks;
	std::map<int, globale>	client_config;
	int			epfd;
	httpPars		parsing;

	int	init_sock(size_t);
	void	epoll_init();
	void	epoll_loop();
	void	accept_client(int sockfd, globale& conf);
	void	_recv(int);
	void	_send(Client&);
	void	recvRq(Client&);
	void	sendRs(Client&);
	void	closeClient(int fd);
	// ``` the parser object ```
	public:
		void run();
};

class SigInt : public std::exception
{
	public:
		const char* what() const throw();
};
