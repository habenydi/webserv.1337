#include "../include.hpp"

void	Server::recv_request(Client& client)
{
	char	buff[client.config.max_client_size];
	memset(buff, 0, sizeof(buff));
	std::string	request;

	ssize_t bytes = recv(client.fd, buff, client.config.max_client_size, 0);
	if (bytes <= 0)
	{
		std::cerr << "[Error] recv fails: " << strerror(errno) << std::endl;
		close_client(client.fd);
		return;
	}
	request.append(buff, bytes);
	std::cout << "[DEBUG] hak request: " << request << std::endl;

	try {
		respons.RequestPars(request, client.config);
	}
	catch (std::runtime_error &e){
		return ;
	}
	catch (...){
		std::cerr << "unexpected error in parsing" << std::endl;
		return ;
	}

	client.ev.events = EPOLLOUT | EPOLLRDHUP;
	epoll_ctl(epfd, EPOLL_CTL_MOD, client.fd, &(client.ev));
}
