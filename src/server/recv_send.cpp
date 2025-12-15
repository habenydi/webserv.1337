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



void	Server::_recv(int fd)
{
	char		buff[client_config[fd].max_client_size]; // 4 kb hhhh
	std::string	request;

	ssize_t byts = recv(fd, buff, client_config[fd].max_client_size, 0);
	if (byts > 0)
		request += buff;
	else
		throw byts;
	parsing.RequestPars(request, client_config[fd]);
}




void	Server::_send(Client& client)
{
	ssize_t byts;
	std::string	&respons = parsing.response;

	if (respons.size() - client.offset > 125000) // 1MB
		byts = send(client.fd, respons.c_str() + client.offset, 125000, MSG_NOSIGNAL);
	else
		byts = send(client.fd, respons.c_str() + client.offset, respons.size() - client.offset, MSG_NOSIGNAL);

	if (byts <= 0)
	{
		closeClient(client.fd);
		return ;
	}

	client.offset += byts;
	if (client.offset >= respons.size())
		closeClient(client.fd);
}
