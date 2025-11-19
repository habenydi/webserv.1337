#pragma once

#include "external.hpp"

class httpPars
{
	private:
		/* data */
		bool	splitHeader(std::string& result, HttpRequest& request);
		void	StoreTheBody(HttpRequest& request, std::string& buffer);
		void	ChunkedBody(HttpRequest& request, std::string& buffer);
		void	RegularBody(HttpRequest& request, std::string& buffer);
	public:
		std::string response;
		bool	RequestPars(std::string& buffer, HttpRequest& request);
};

void creat_and_write(std::string& dirname, std::string& content);
std::string generateResponse(const HttpRequest& request);
