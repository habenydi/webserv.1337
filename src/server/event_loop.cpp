#include "../include.hpp"

void	set_non_blocking(int);

void	Server::accept_client(Sockets& sock)//, struct epoll_event& ev)
{
	int	fd = accept(sock.sockfd, NULL, NULL);
	if (fd > 0)
	{
		set_non_blocking(fd);

		Client	client;
		client.fd		= fd;
		client.config		= sock.config;

		client.bodyTime = 0;
		client.readTime = 1;
		client.offset = 0;
		client.readed = 0;
		client.wasSent = 0;


		client.ev.events	=  EPOLLIN | EPOLLRDHUP;
		client.ev.data.fd	= fd;

		_clients[fd]	= client;

		epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &(client.ev));
	}
}

void	Server::epoll_init()
{
	epfd = epoll_create(1);
	if (epfd == -1)	{
		std::cerr << "[Error] epoll_create fails: " << strerror(errno) << std::endl;
		return;
	}

	for (size_t i = 0; i < _sockets.size(); i++)
	{
		struct epoll_event	ev;
		ev.events = EPOLLIN;
		ev.data.fd = _sockets[i].sockfd;
		epoll_ctl(epfd, EPOLL_CTL_ADD, _sockets[i].sockfd, &ev);
		std::cout << "Listening in the port: " << _sockets[i].port << std::endl;
	}
}

void	Server::event_loop()
{
	epoll_init();
	while (true)
	{
		struct epoll_event	clients[100];
		int	n = epoll_wait(epfd, clients, 100, -1);
		if (n == -1) {
			if (errno == EINTR) continue; // Interrupted by signal
			std::cerr << "[Error] epoll_wait fails: " << strerror(errno) << std::endl;
			break;
		}
		
		for (int i = 0; i < n; i++)
		{
			int	efd = clients[i].data.fd;
			int	skip = 0;

			/////////////////////////////////////
			for (size_t j = 0; j < _sockets.size(); j++)
			{
				if (efd == _sockets[j].sockfd)
				{
					accept_client(_sockets[j]);//, clients[i]);
					std::cout << "[DEBUG] client accepted" << std::endl;
					skip = 1;
					break;
				}
			}
			if (skip) continue;
			/////////////////////////////////////
			if (clients[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
			{
				close_client(efd);
				std::cout << "[DEBUG] the connection closed 1" << std::endl;
				continue;
			}
			//////////////////////////////////////
			if (_clients.find(efd) == _clients.end())
			{
			    std::cerr << "[ERROR] Unknown client fd " << efd << std::endl;
			    close_client(efd);
			    continue;
			}
			
			if (clients[i].events & EPOLLIN)
				recv_request(_clients[efd]);
			else if (clients[i].events & EPOLLOUT)
				send_response(_clients[efd]);
					
		}
	}
}

void	Server::close_client(int fd)
{
	epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
	close(fd);
	_clients.erase(fd);
}

