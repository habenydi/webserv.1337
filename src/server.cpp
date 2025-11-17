#include "include.hpp"

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
