#pragma once

#include "external.hpp"

class httpPars
{
private:
	/* data */
public:
	bool	RequestPars(int fd, HttpRequest& request);
	bool	splitHeader(std::string& result, HttpRequest& request);
	bool	splitBody(std::string& result, HttpRequest& request, int& fd);
	bool	RespansePars(int fd, HttpRequest& request);
};
