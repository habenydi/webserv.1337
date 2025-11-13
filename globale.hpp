#pragma once

#include "include.hpp"

struct LocationConfig
{
    std::string path;
    std::string root;
    bool autoindex;
    std::vector<std::string> allowed_methods;
    std::string upload_store;
    std::string cgi_extension;
};

class globale
{
	public:
		std::vector <std::string>	port;
		std::string	server_name;
		std::string path;
		std::string	root;
		unsigned long max_client_size;
		bool autoindex;
		std::vector<std::string> index;
		std::map<int, std::string> error_page;
		std::vector<LocationConfig> location;
		globale& operator=(const globale& other)
		{
			if (this != &other)
			{
				this->path = other.path;
				this->port = other.port;
				this->root = other.root;
				this->autoindex = other.autoindex;
				this->index = other.index;
				this->error_page = other.error_page;
				this->location = other.location;
				// if you add any thing int the private of class you have to add here
			}
			return (*this);
		}
};

