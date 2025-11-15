#pragma once

#include "include.hpp"

class httpPars
{
private:
	/* data */
public:
	httpPars(/* args */);
	void    RequestPars(int fd, HttpRequest& request)
	{
		(void)fd;
		(void)request;
	}
	void    RespansePars(int fd, HttpRequest& request)
	{
		(void)fd;
		(void)request;
	}
	~httpPars();
};

httpPars::httpPars(/* args */)
{
}

httpPars::~httpPars()
{
}
