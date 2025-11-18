#include "include.hpp"

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
	char		buff[4096]; // 4 kb hhhh
	std::string	request;
	ssize_t byts = recv(fd, buff, 4096, 0);

	if (byts > 0)
	{
		request += buff;
		std::cout << "-------------------------------------------------" << std::endl;
		std::cout << request << std::endl;
		throw std::string("ready");
	}
	else
		throw byts;
	// 	parce_http(request);
}

void	Server::_send(Client& client)
{
	std::string	respons;// will come from parser


	std::cout << "-=-=================[DEBUG] time to send=================-=-" << std::endl;//
	std::string	line;
	std::ifstream	thefile("index.html");
	if (!thefile.is_open() || thefile.peek() == -1)
	{
		std::cout << "the file did not opened or empty\n";
		return ;
	}

	std::string	body((std::istreambuf_iterator<char>(thefile)),std::istreambuf_iterator<char>());
	respons =
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + to_string98(body.size()) + "\r\n"
        "\r\n" +
        body;
	std::cout << "[DEBUG]\n" << respons << std::endl;



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
