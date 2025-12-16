#pragma once

#include "../../external.hpp"

struct LocationConfig
{
	std::string path;
	std::string root;
	bool autoindex;
	std::vector<std::string> allowed_methods;
	std::string upload_store;
	std::string cgi_extension;
	std::vector<std::string> index;
	std::string cgi_pass;
	// Constructor: initialize all members
	LocationConfig()
		: path(), root(), autoindex(false), allowed_methods(),
		  upload_store(), cgi_extension(), index(), cgi_pass()
	{
	}
	LocationConfig&	operator=(const LocationConfig& other)
	{
		if (this != &other)
		{
			this->path = other.path;
			this->root = other.root;
			this->autoindex = other.autoindex;
			this->allowed_methods = other.allowed_methods;
			this->upload_store = other.upload_store;
			this->cgi_extension = other.cgi_extension;
			this->index = other.index;
			this->cgi_pass = other.cgi_pass;
		}
		return *this;
	}
};

class globale
{
	public:
		std::vector<int> port;
		std::string host;
		std::string server_name;
		std::string path;
		std::string root;
		unsigned long max_client_size;
		bool autoindex;
		std::string index;
		std::map<int, std::string> error_page;
		std::vector<LocationConfig> location;
		std::map<std::string , std::string> interpreters;
		std::multimap<std::string, std::string>  mimetype;
		// Constructor: initialize all members
		globale()
			: port(), host(), server_name(), path(), root(),
			max_client_size(0), autoindex(false), index(),
			error_page(), location(), mimetype()
		{
		}

		globale &operator=(const globale &other)
		{
			if (this != &other)
			{
				this->path = other.path;
				this->port = other.port;
				this->host = other.host;
				this->root = other.root;
				this->autoindex = other.autoindex;
				this->index = other.index;
				this->error_page = other.error_page;
				this->location = other.location;
				this->server_name = other.server_name;
				this->max_client_size = other.max_client_size;
				this->interpreters = other.interpreters;
				this->mimetype = other.mimetype;
			}
			return (*this);
		}
};
