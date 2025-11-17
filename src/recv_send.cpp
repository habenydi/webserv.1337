#include "include.hpp"

std::string to_string98(size_t n)
{
    std::ostringstream oss;
    oss << n;
    return oss.str();
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

void	Server::_send(int fd)
{
	std::string	respons;// will come from parser


		std::cout << "-=-=================[DEBUG] time to send=================-=-" << std::endl;//
	std::string	body;//
	std::string	line;//
	static size_t		offset;

	std::ifstream	thefile("index.html");//
	if (!thefile.is_open() || thefile.peek() == -1)//
	{//
		std::cout << "the file did not opened or empty\n";//
		return ;//
	}//
	while (std::getline(thefile, line))//
	{//
		std::cout << "[DEBUG] kayn" << std::endl;//
		body.append(line);//
		body.append("\n");//
	}
	respons =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: " + to_string98(body.size()) + "\r\n"
        "\r\n" +
        body;
	std::cout << "-------------------------------------------------\n[DEBUG]" << respons << std::endl;//
													  //
													  //
													  //
	ssize_t byts = send(fd, respons.c_str() + offset, respons.length() - offset, 0);
	if (byts > 0)
	{
		offset += byts;
		if (offset >= respons.length())
			throw std::string("done");
	}else
		throw byts;

}
