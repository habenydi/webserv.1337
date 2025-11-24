#pragma once

#include "../../external.hpp"
#include "../conf_file/globale.hpp"

class httpPars
{
	private:
		bool	splitHeader(std::string& result);
		void	StoreTheBody(std::string& buffer);
		void	ChunkedBody(std::string& buffer);
		void	FindFilename(HttpRequest& request);
		void	RegularBody(std::string& buffer);
		void	StoreCookies(std::string& lines);
	public:
		HttpRequest	request;
		std::string response;
		globale	conf;
		bool	RequestPars(std::string& buffer, globale& conf);
};

void creat_and_write(std::string& name, std::string& content);
std::string generateResponse(const HttpRequest& request);
