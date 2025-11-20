#pragma once

#include "../external.hpp"

enum	scripts
{
	py = 0,
	js = 1
};

class	CGI
{
	void	python();
	void	javaScript();
	void	runScript(std::string, std::string);
	public:
		std::string	output;

		CGI(int);
};
