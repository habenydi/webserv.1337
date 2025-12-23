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



void	Server::send_body(Client& client)
{
	int	fd = parsing.response.fd;
	char	body[MB(1)];
	ssize_t	&readed = client.readed;

	if (client.readTime)
	{
		readed = read(fd, body, MB(1)); // 1MB
		if (readed <= 0)
		{
			std::cout << "[DEBUG] waaaaaaaaa333 .. read failaaat" << std::endl;
			close(fd);
			closeClient(client.fd);
			client.bodyTime = 0;
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


	std::cout << "\033[38;2;255;0;0m[DEBUG] was Sent: " << client.wasSent << "\033[0m" << std::endl;
	client.wasSent = client.wasSent + sent;
	std::cout << "\033[38;2;255;0;0m[DEBUG] Sent: " << sent << "\033[0m" << std::endl;
	if ((size_t)client.wasSent >= parsing.response.size)
	{
		std::cout << "\033[38;2;255;0;0m[DEBUG] safi kolshi Sala" << std::endl;
		std::cout << "        wasSent: " << client.wasSent << std::endl;
		std::cout << "        expected: " << parsing.response.size << "\033[0m" << std::endl;
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
