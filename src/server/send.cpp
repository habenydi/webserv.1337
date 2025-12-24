#include "../include.hpp"

void	Server::send_response(Client& client)
{
	std::string	&header = respons.response.header;

	if (!client.bodyTime){
		send(client.fd, header.c_str(), header.size(), MSG_NOSIGNAL);
		client.bodyTime = 1;
	}

	if (respons.response.fd > 0)
		send_body(client);
	else
	{
		client.bodyTime = 0;
		close_client(client.fd);
	}
}

void	Server::send_body(Client& client)
{
	std::cout << "[DEBUG] Time TO Send" << std::endl;
	int	file_fd = respons.response.fd;
	char	body[MB(1)];
	ssize_t	&readed = client.readed;

	if (client.readTime)
	{
		readed = read(file_fd, body, MB(1)); // 1MB
		if (readed <= 0)
		{
			std::cerr << "[ERROR] read() fails: " << std::endl;
			close(file_fd);
			close_client(client.fd);
			client.bodyTime = 0;
			return ;
		}
		client.readTime = 0;
		client.offset = 0;
	}

	ssize_t		sent;
	sent = send(client.fd, body + client.offset, readed - client.offset, MSG_NOSIGNAL);
	if (sent <= 0)
	{
		close(file_fd);
		close_client(client.fd);
		return;
	}

	client.offset += sent;
	if (client.offset >= readed)
		client.readTime = 1;

	client.wasSent = client.wasSent + sent;
	if ((size_t)client.wasSent >= respons.response.size)
	{
		close(file_fd);
		close_client(client.fd);
		client.bodyTime = 0;
	}

}
