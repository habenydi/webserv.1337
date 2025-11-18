#pragma once

#include "external.hpp"

class httpPars
{
private:
	/* data */
public:
	bool	RequestPars(std::string& buffer, HttpRequest& request);
	bool	splitHeader(std::string& result, HttpRequest& request);
	bool	splitBody(std::string& result, HttpRequest& request);
	bool	RespansePars(int fd, HttpRequest& request);
};
