#pragma once

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <fstream>
#include <exception>
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <csignal>
#include <netinet/in.h>
#include <ostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAX_REQUEST_SIZE 5362

struct HttpRequest
{
	// --- Request Line ---
	std::string method;   // GET, POST, DELETE
	std::string path;     // e.g., "/index.html"
	std::string version;  // HTTP/1.1

	// --- Headers ---
	std::map<std::string, std::string> headers; // header_name -> header_value

	// --- Body ---
	std::string body;     // raw body of the request

	// --- Parsed info for convenience (bonus) ---
	std::string query_string;               // if URL has "?..."
	std::map<std::string, std::string> cookies; // parsed "Cookie" header
	std::map<std::string, std::string> form_data; // for application/x-www-form-urlencoded POST
	std::vector<unsigned char> raw_body;    // if binary upload, store as bytes
	bool IsPOST;
};
