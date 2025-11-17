#include "include.hpp"

void	epoll_init(int epfd, int sockfd)
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

void	Server::epoll_loop()
{
	//epoll_init(epfd, sockfd);

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
					std::cout << "[DEBUG] the connection closed 1" << std::endl;
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
							std::cout << "[DEBUG] ready to send" << std::endl;
							clients[i].events = EPOLLOUT | EPOLLRDHUP;
							epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &(clients[i]));
							continue;
						}
					}catch (ssize_t e)
					{
						epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
						close(fd);
						std::cout << "[DEBUG] the connection closed 2" << std::endl;
						continue;
					}
				}
				else if (clients[i].events & EPOLLOUT)
				{
					std::cout << "[DEBUG] time to send" << std::endl;
					try{
						_send(fd);
					}catch (std::string e)
					{
						clients[i].events = EPOLLIN | EPOLLRDHUP;
						epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &(clients[i]));
					}catch (ssize_t e)
					{
						epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
						close(fd);
						std::cout << "[DEBUG] the connection closed 3" << std::endl;
						continue;
					}
				}
			}
		}
	}
}

