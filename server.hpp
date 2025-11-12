#pragma once

#include "include.hpp"

class	Server
{
	std::vector<struct pollfd> _pollFds;
	int	init_first_sock(int port);
	public:
		Server();
		~Server();
		void run(int port);
};
