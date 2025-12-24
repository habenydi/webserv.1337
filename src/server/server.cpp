#include "../include.hpp"

void	Server::run(std::vector<globale>& configs)
{
	for (size_t i = 0; i < configs.size(); i++)
	{
		for (size_t j = 0; j < configs[i].port.size(); j++)
		{
			Socket	sock;
			
			if (init_socket(configs[i].port[j], configs[i]))
				return ;
		}
	}


	event_loop();
}

int set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int	binding(Socket& sock, globale& config)
{
	struct sockaddr_in baddr;
	memset(&baddr, 0, sizeof(baddr));
	baddr.sin_family = AF_INET;
	baddr.sin_port = htons(sock.port);
	baddr.sin_addr.s_addr = inet_addr(config.host.c_str());
	if (bind(sock.sockfd, (struct sockaddr*)&baddr, sizeof(baddr)))
	{
		std::cerr << "[Error] bind fails: " << strerror(errno) << std::endl;
		return -1;
	}
	return 0;
}


int	Server::init_socket(int port, globale& config)
{
	Socket	sock;

	sock.port = port;
	sock.config = config;
	sock.sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock.sockfd == -1){
		std::cerr << "socket() fails: "  << strerror(errno) << std::endl;
		return -1;
	}

	int opt = 1;
	setsockopt(sock.sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	set_non_blocking(sock.sockfd);

	if (binding(sock, config))
		return -1;

	if (listen(sock.sockfd, 10)){
		std::cerr << "[ERROR] Listen fails: " << strerror(errno) << std::endl;
		return -1;
	}


	_sockets.push_back(sock);
	return 0;
}
