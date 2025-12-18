#include "../include.hpp"

void	Server::recvRq(Client& client)
{
	try{
		_recv(client.fd);
	}catch (std::runtime_error& e)
	{
		return;
	}catch (ssize_t e)
	{
		closeClient(client.fd);
		std::cout << "[DEBUG] the connection closed 2" << std::endl;
	}
	std::cout << "[DEBUG] ready to send" << config[0].port[0] << std::endl;
	client.ev.events = EPOLLOUT | EPOLLRDHUP;
	epoll_ctl(epfd, EPOLL_CTL_MOD, client.fd, &(client.ev));
}



void	Server::epoll_init()
{
	epfd = epoll_create(1);
	if (epfd == -1)
	{
		std::cerr << "[Error] epoll_create fails" << std::endl;
		return;
	}

	for (size_t i = 0; i < socks.size(); i++) {
		struct	epoll_event ev;
		ev.events = EPOLLIN;
		ev.data.fd = socks[i].sockfd;
		epoll_ctl(epfd, EPOLL_CTL_ADD, socks[i].sockfd, &ev);
		std::cout << "Listening in the port: " << socks[i].port << std::endl;
	}
}




void	Server::epoll_loop()
{
	epoll_init();
	while (true)
	{
		struct epoll_event clients[1000];
		int n = epoll_wait(epfd, clients, 1000, -1);
		if (n == -1) {
		    if (errno == EINTR) continue; // Interrupted by signal
		    std::cerr << "[Error] epoll_wait failed" << std::endl;
		    break;
		}

		for (int i=0; i < n; i++)
		{
			int f = 0;
			Client	client;
			client.offset = 0;
			client.ev = clients[i];
			client.fd = clients[i].data.fd;

			for (size_t i = 0; i < socks.size(); i++) {
				if ((int)client.fd == socks[i].sockfd)
				{
					accept_client(socks[i].sockfd, socks[i].conf);
					f = 1;
					break;
				}
			}
			if (f) continue;

			if (client.ev.events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
			{
				closeClient(client.fd);
				std::cout << "[DEBUG] the connection closed 1" << std::endl;
				continue;
			}
			else if (clients[i].events & EPOLLIN)
				recvRq(client);
			else if (clients[i].events & EPOLLOUT)
				_send(client);

		}
	}
}
