#pragma once

#include "../../external.hpp"

class httpPars
{
	private:
		bool	splitHeader(std::string& result);
		void	StoreTheBody(std::string& buffer);
		void	ChunkedBody(std::string& buffer);
		void	RegularBody(std::string& buffer);
	public:
		HttpRequest	request;
		std::string response;
		globale	conf;
		bool	RequestPars(std::string& buffer, globale& configue);
};

void creat_and_write(std::string& dirname, std::string& content);
std::string generateResponse(const HttpRequest& request);
