#include "include.hpp"
#include <csignal>
#include <iostream>
#include <netinet/in.h>
#include <ostream>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "server.hpp"

void	Server::run(int port)
{
	if (init_first_sock(port))
		return;
	epoll_loop();
}

int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int	Server::init_first_sock(int port)
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
	    std::cerr << "socket() failed" << std::endl;
	    return -1;
	}
	int opt = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // for "Address already in use" problem
	set_nonblocking(sockfd);

	struct sockaddr_in baddr;
	memset(&baddr, 0, sizeof(baddr));
	baddr.sin_family = AF_INET;
	baddr.sin_port = htons(port);
	baddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sockfd, (struct sockaddr*)&baddr, sizeof(baddr)))
	{
		std::cerr << "[Error] bind fails" << std::endl;
		return -1;
	}
	if (listen(sockfd, 10))
	{
		std::cerr << "[Error] listen fails" << std::endl;
		return -1;
	}
	// ghayt7ayed
	_port = port;
	return 0;
}

void	Server::accept_client()
{
	int client_fd = accept(sockfd, NULL, NULL);
	if (client_fd != -1) {
                set_nonblocking(client_fd);
                _fds.push_back(client_fd);

                struct epoll_event ev;
                ev.events = EPOLLIN | EPOLLRDHUP;
                ev.data.fd = client_fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
	}
}

void	Server::_recv(int fd)
{
	char		buff[4096]; // 4 kb hhhh
	std::string	request;
	ssize_t byts = recv(fd, buff, 1024, 0);

	if (byts > 0)
		request += buff;
	else
		throw byts;
	// 	parce_http(request);
}

void	Server::_send(int fd)
{
	std::string	respons;// will come from parser
	static size_t		offset;

	ssize_t byts = send(fd, respons.c_str() + offset, respons.length() - offset, 0);
	if (byts > 0)
	{
		offset += byts;
		if (offset >= respons.length())
			throw std::string("done");
	}else
		throw byts;

}

void	Server::epoll_loop()
{
	epfd = epoll_create(1);
	if (-1 == epfd)
		std::cerr << "[Error] epoll_create fails" << std::endl;return;

	std::cout << "[DEBUG] sockfd: " << sockfd << std::endl;


	struct	epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = sockfd;

	epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);

	while (true)
	{
		struct epoll_event clients[1000];
		int n = epoll_wait(epfd, clients, 1000, -1);

		for (int i=0; i < n; i++)
		{
			int fd = clients[i].data.fd;
			if (fd == sockfd)
				accept_client();
			else
			{
				if (clients[i].events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP))
				{
					epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
					close(fd);
					continue;
				}
				else if (clients[i].events & EPOLLIN)
				{
					try{
						_recv(fd);
					}catch (std::string e)
					{
						if (e == "ready")
						{
							ev.events = EPOLLOUT | EPOLLRDHUP;
							epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
						}
					}catch (ssize_t e)
					{
						epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
						close(fd);
						continue;
					}
				}
				else if (clients[i].events & EPOLLOUT)
				{
					try{
						_send(fd);
					}catch (std::string e)
					{
						ev.events = EPOLLIN | EPOLLRDHUP;
						epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev);
					}catch (ssize_t e)
					{
						epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
						close(fd);
						continue;
					}
				}
			}
		}
	}
}

