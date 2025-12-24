#pragma once

#include "../external.hpp"
#include "../parsing/httpRequest/httpPars.hpp"
#include "../parsing/conf_file/globale.hpp"

typedef	struct	Socket
{
	int	sockfd;
	int	port;
	globale	config;

}	Sockets;

typedef struct	Client
{
	struct	epoll_event	ev;
	int			fd;
	globale			config;

	bool			bodyTime;
	bool			readTime;
	ssize_t			offset;
	ssize_t			readed;
	ssize_t			wasSent;

	Client() : bodyTime(0), readTime(1), offset(0), readed(0), wasSent(0) {}
}	Client;

class	Server
{
	int			epfd;
	httpPars		respons;
	std::vector<Socket>	_sockets;
	std::map<int, Client>	_clients;

	void	event_loop(void);
	void	send_body(Client& client);
	void	epoll_init(void);
	void	close_client(int);
	void	accept_client(Socket&);//, struct epoll_event&);
	void	recv_request(Client&);
	void	send_response(Client&);
	int	init_socket(int port, globale& config);
	public:
		void	run(std::vector<globale>&);
};


class SigInt : public std::exception
{
	public:
		const char* what() const throw();
};
