#include "../include.hpp"

 
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



/*
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
}*/














void	Server::send_body(Client& client)
{
	int	fd = parsing.response.fd;
	char	body[1048576];
	ssize_t	&readed = client.readed;

	if (client.readTime)
	{
		readed = read(fd, body, 1048576); // 1MB
		if (readed <= 0)
		{
			std::cout << "[DEBUG] waaaaaaaaa333 .. read failaaat" << std::endl;
			close(fd);
			closeClient(client.fd);
			return;
		}
		std::cout << "[DEBUG] 9raya salat" << std::endl;
		std::cout << "        9rit: " << readed << std::endl;
		client.readTime = 0;
		client.offset = 0;
	}

	ssize_t		sent;
	sent = send(client.fd, body + client.offset, readed - client.offset, MSG_NOSIGNAL);

	if (sent <= 0)
	{
		close(fd);
		closeClient(client.fd);
		return ;
	}

	client.offset += sent;
	if (client.offset >= readed)
	{
		std::cout << "[DEBUG] Send salat" << std::endl;
		std::cout << "        Sendit: " << sent << std::endl;
		std::cout << "        loffset: " << client.offset << std::endl;
		client.readTime = 1;
		client.offset = 0;
	}


	client.wasSent += sent;
	if ((size_t)client.wasSent >= parsing.response.size)
	{
		std::cout << "[DEBUG] safi kolshi Sala" << std::endl;
		std::cout << "        wasSent: " << client.wasSent << std::endl;
		std::cout << "        expected: " << parsing.response.size << std::endl;
		//send(client.fd, "\r\n", 2, MSG_NOSIGNAL);
		close(fd);
		closeClient(client.fd);
		client.bodyTime = 0;
	}
}


void	Server::_send(Client& client)
{
	std::string	&header = parsing.response.header;
	// TODO:  loffset wkda bash mat assumish bli send sendat lheader caml
	

	if (!client.bodyTime){
		send(client.fd, header.c_str(), header.size(), MSG_NOSIGNAL);
		client.bodyTime = 1;
	}
	if (parsing.response.fd > 0)
		send_body(client);
	else
	{
		client.bodyTime = 0;
		closeClient(client.fd);
	}
}
