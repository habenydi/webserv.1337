#include "../include.hpp"
#include <iostream>
#include <ostream>

/*std::string to_string98(size_t n)
{
    std::ostringstream oss;
    oss << n;
    return oss.str();
}*/



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
	static bool		f = 0;
	char		body[125000];
	int		fd = parsing.response.fd;
	std::string	&header = parsing.response.header;

	if (f == 0)
		byts = send(client.fd, header.c_str() + client.offset, header.size() - client.offset, MSG_NOSIGNAL);

	client.offset += byts;
	if (client.offset >= header.size()) { std::cout << "  Sala lheader " << std::endl; f = 1; client.offset = 0; }


	if (fd > 0 && read(fd, body, 125000))
	{
		byts = send(client.fd, body, std::string(body).size(), MSG_NOSIGNAL);
		if (byts <= 0)
		{
			closeClient(client.fd);
			return ;
		}
	//	client.offset += byts;
	//} else if (fd < 0)// && !body.empty())
	//{
		// TODO: send the string body 
		// WARNING: 3ndak maydozsh llelse l5ra
	} else
	{
		send(client.fd, "\r\n", 2, MSG_NOSIGNAL);
		std::cout << "  Sala lBody " << std::endl;
		f = 0;
		closeClient(client.fd);
	}
}
