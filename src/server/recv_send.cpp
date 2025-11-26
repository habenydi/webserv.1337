#include "../include.hpp"

std::string to_string98(size_t n)
{
    std::ostringstream oss;
    oss << n;
    return oss.str();
}

void	Server::closeClient(int fd)
{
	epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
	close(fd);
}

void	Server::_recv(int fd, globale& conf)
{
	char		buff[conf.max_client_size]; // 4 kb hhhh
	std::string	request;

	ssize_t byts = recv(fd, buff, conf.max_client_size, 0);
	if (byts > 0)
	{
		request += buff;
	}
	else
		throw byts;
	parsing.RequestPars(request, conf);
}

void	Server::_send(Client& client)
{
	std::string	respons = parsing.response;// will come from parser

	ssize_t byts = send(client.fd, respons.c_str() + client.offset, respons.length() - client.offset, MSG_NOSIGNAL);

	if (byts <= 0)
	{
		closeClient(client.fd);
		return ;
	}

	client.offset += byts;
	if (client.offset >= respons.length())
		closeClient(client.fd);
}
