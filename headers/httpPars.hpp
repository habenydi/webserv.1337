#pragma once

#include "external.hpp"

class httpPars
{
private:
	/* data */
public:
	bool	RequestPars(std::string& buffer, HttpRequest& request);
	bool	splitHeader(std::string& result, HttpRequest& request);
	void	StoreTheBody(HttpRequest& request, std::string& buffer);
	void	ChunkedBody(HttpRequest& request, std::string& buffer);
	void	RegularBody(HttpRequest& request, std::string& buffer);
};
