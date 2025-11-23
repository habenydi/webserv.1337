#include "../include.hpp"

void	Server::run()
{
	for (size_t i = 0; i < config.size(); i++) {
		for (size_t j = 0; j < config[i].port.size(); j++) {
			sock	awdi;
			awdi.port = config[i].port[j];
			awdi.conf = config[i];
			socks.push_back(awdi);
		}
	}
	std::cout << "[DEBUG] socks size: " << socks.size() << std::endl;
	if (init_first_sock())
		return;
	epoll_loop();
}

int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int	Server::init_first_sock()
{
	for (size_t i = 0; i < socks.size(); i++)
	{
		socks[i].sockfd	= socket(AF_INET, SOCK_STREAM, 0);
		if (socks[i].sockfd == -1)
		{
		    std::cerr << "socket() failed" << std::endl;
		    return -1;
		}
		int opt = 1;
		setsockopt(socks[i].sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // for "Address already in use" problem
		set_nonblocking(socks[i].sockfd);

		struct sockaddr_in baddr;
		memset(&baddr, 0, sizeof(baddr));
		baddr.sin_family = AF_INET;
		baddr.sin_port = htons(socks[i].port);
		baddr.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bind(socks[i].sockfd, (struct sockaddr*)&baddr, sizeof(baddr)))
		{
			std::cerr << "[Error] bind fails" << std::endl;
			return -1;
		}
		if (listen(socks[i].sockfd, 10))
		{
			std::cerr << "[Error] listen fails" << std::endl;
			return -1;
		}
	}
	return 0;
}

void	Server::accept_client(int sockfd)
{
	int client_fd = accept(sockfd, NULL, NULL);
	if (client_fd != -1) {
                set_nonblocking(client_fd);

                struct epoll_event ev;
                ev.events = EPOLLIN | EPOLLRDHUP;
                ev.data.fd = client_fd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
	}
}
