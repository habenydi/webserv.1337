#include "../include.hpp"
#include <string>

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
	ssize_t		byts = 0;
	bool		f = 0;
	char		buff[125000];
	int		body = parsing.response.fd;
	std::string	&header = parsing.response.header;

	if (!f)
		byts = send(client.fd, header.c_str() + client.offset, header.size() - client.offset, MSG_NOSIGNAL);

	client.offset += byts;
	if (client.offset >= header.size()) { f = 1; client.offset = 0; }


	if (read(body, buff, 125000))
	{
		byts = send(client.fd, buff + client.offset, std::string(buff).size(), MSG_NOSIGNAL);
//			byts = send(client.fd, respons.c_str() + client.offset, respons.size() - client.offset, MSG_NOSIGNAL);

		if (byts <= 0)
		{
			closeClient(client.fd);
			return ;
		}
		client.offset += byts;
	} else
	{
		send(client.fd, "\r\n", 2, MSG_NOSIGNAL);
		closeClient(client.fd);
	}

	//if (client.offset >= header.size())
}
