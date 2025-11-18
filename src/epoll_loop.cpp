#include "include.hpp"
#include <map>

void	epoll_init(int& epfd, int sockfd)
{
	epfd = epoll_create(1);
	if (epfd == -1)
	{
		std::cerr << "[Error] epoll_create fails" << std::endl;
		return;
	}

	struct	epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = sockfd;
	epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
}

void	Server::recvRq(struct epoll_event& ev, int fd)
{
	try{
		_recv(fd);
	}catch (std::string e)
	{
		if (e == "ready")
		{
			std::cout << "[DEBUG] ready to send" << std::endl;
			ev.events = EPOLLOUT | EPOLLRDHUP;
			epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &(ev));
		}
	}catch (ssize_t e)
	{
		closeClient(fd);
		std::cout << "[DEBUG] the connection closed 2" << std::endl;
	}
}

void	Server::epoll_loop()
{
	epoll_init(epfd, sockfd);
	//std::map<int, Client> _Clients;
	while (true)
	{
		struct epoll_event clients[1000];
		int n = epoll_wait(epfd, clients, 1000, -1);
		for (int i=0; i < n; i++)
		{
			int fd = clients[i].data.fd;
			Client	client;
			client.offset = 0;
			client.ev = clients[i];
			client.fd = clients[i].data.fd;
			//_Clients[fd] = client;

			if (fd == sockfd)
				accept_client();
			else
			{
				if (client.ev.events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
				{
					closeClient(fd);
					std::cout << "[DEBUG] the connection closed 1" << std::endl;
					continue;
				}
				else if (clients[i].events & EPOLLIN)
					recvRq(client.ev, fd);
				else if (clients[i].events & EPOLLOUT)
					_send(client);
			}
		}
	}
}

