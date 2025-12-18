#include "../include.hpp"
#include <cstring>
#include <iostream>
#include <ostream>

void	Server::run()
{
	for (size_t i = 0; i < config.size(); i++) {
		for (size_t j = 0; j < config[i].port.size(); j++) {
			sock	socket;
			socket.port = config[i].port[j];
			socket.conf = config[i];
			socks.push_back(socket);
		}
	}
	std::cout << "[DEBUG] socks size: " << socks.size() << std::endl;
	for (size_t i = 0; i < socks.size(); i++)
		if (init_sock(i))
			return;
	epoll_loop();
}




int set_nonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}




int	Server::init_sock(size_t i)
{
	socks[i].sockfd	= socket(AF_INET, SOCK_STREAM, 0);
	if (socks[i].sockfd == -1)
	{
	    std::cerr << "socket() failed" << std::endl;
	    return -1;
	}

	int opt = 1;
	setsockopt(socks[i].sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // for "Address already in use" problem
	set_nonBlocking(socks[i].sockfd);

	struct sockaddr_in baddr;
	memset(&baddr, 0, sizeof(baddr));
	baddr.sin_family = AF_INET;
	baddr.sin_port = htons(socks[i].port);
//	std::cout << "[DEBUG] lIP : " << socks[i].conf.host << std::endl;
//	std::cout << "[DEBUG] linet_addr : " << inet_addr(socks[i].conf.host.c_str()) << std::endl;
	baddr.sin_addr.s_addr = inet_addr(socks[i].conf.host.c_str()); // TODO: turn this INADDR_ANY to the host ip.. so socks[i].conf.host
	if (bind(socks[i].sockfd, (struct sockaddr*)&baddr, sizeof(baddr)))
	{
		std::cerr << "[Error] bind fails: " << strerror(errno) << std::endl;
		return -1;
	}

	if (listen(socks[i].sockfd, 10))
	{
		std::cerr << "[Error] listen fails: "  << strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}




void	Server::accept_client(int sockfd, globale& conf)
{
	int client_fd = accept(sockfd, NULL, NULL);
	client_config[client_fd] = conf;
	if (client_fd != -1) {
                set_nonBlocking(client_fd);

                struct epoll_event ev;
                ev.events = EPOLLIN | EPOLLRDHUP;
                ev.data.fd = client_fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
	}
}
