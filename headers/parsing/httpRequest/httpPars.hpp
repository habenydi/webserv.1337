#pragma once

#include "../../external.hpp"
#include "../conf_file/globale.hpp"

struct Response
{
	std::string header;
	int fd;
	size_t size;

	Response() : fd(-1), size(0) {}
};

struct HttpRequest
{
	// --- Request Line ---
	std::string method;	 // GET, POST, DELETE
	std::string path;	 // e.g., "/index.html"
	std::string version; // HTTP/1.1
	globale conf;

	// --- Headers ---
	std::map<std::string, std::string> headers; // header_name -> header_value

	// --- Body ---
	std::string body; // raw body of the request

	// --- Parsed info for convenience (bonus) ---
	std::string query_string;					  // if URL has "?..."
	std::map<std::string, std::string> cookies;	  // parsed "Cookie" header
	std::map<std::string, std::string> form_data; // for application/x-www-form-urlencoded POST
	std::vector<unsigned char> raw_body;		  // if binary upload, store as bytes
	std::map<std::string, Session>	Sid;
	bool IsPOST;
};

class httpPars
{
private:
	bool splitHeader(std::string &result);
	void StoreTheBody(std::string &buffer);
	void ChunkedBody(std::string &buffer);
	void FindFilename(HttpRequest &request);
	void RegularBody(std::string &buffer);
	void StoreCookies(std::string &lines);

public:
	HttpRequest request;
	Response response;
	bool RequestPars(std::string &buffer, globale &conf);
};

void creat_and_write(std::string &name, std::string &content);
Response generateResponse(const HttpRequest &request);
