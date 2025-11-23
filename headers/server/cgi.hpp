#pragma once

#include "../external.hpp"

class	CGI
{
	HttpRequest _req;
	std::string _root;

	void	setEnv(std::string&);

	public:
		std::string	output;

		CGI(std::string, HttpRequest);
		void	run(std::string, std::string, std::string);
};
