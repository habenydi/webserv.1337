#include "../headers/httpPars.hpp"

bool	httpPars::splitHeader(std::string& result, HttpRequest& request)
{
	if (result.find("\r\n\r\n") == std::string::npos)
		return false;
	std::vector <std::string> lines;
	size_t next , pos = 0;
	while ((next = result.find("\r\n", pos)) != std::string::npos)
	{
		lines.push_back(result.substr(pos, next - pos));
		pos = next + 2;
	}
	if (pos < result.size())
		lines.push_back(result.substr(pos));
	if (lines.empty())
		return false;
	std::istringstream first(lines[0].c_str());
	first >> request.method >> request.path >> request.version;
	if (request.path.empty() || request.method.empty() || request.version.empty())
		return false;
	if (request.method == "POST")
		request.IsPOST = true;
	for (size_t i = 1; i < lines.size(); i++)
	{
		if ((pos = lines[i].find(":")) == std::string::npos)
			continue;
		std::string key = lines[i].substr(0, pos);
		std::string value = lines[i].substr(pos + 1);
		key.erase(0, key.find_first_not_of(" \t"));
		key.erase(key.find_last_not_of(" \t") + 1);
		value.erase(0, value.find_first_not_of(" \t"));
		value.erase(value.find_last_not_of(" \t") + 1);
		request.headers[key] = value;
	}
	std::map<std::string, std::string>::iterator it;
	for (it = request.headers.begin(); it != request.headers.end(); ++it)
	{
		std::cout << '"' << it->first << '"' 
				<< " <--> "
				<< '"' << it->second << '"' 
				<< std::endl;
	}
	return true;
	
}

bool	httpPars::RequestPars(std::string& buffer, HttpRequest& request)
{
	request.IsPOST = false;
	size_t header_end = buffer.find("\r\n\r\n");
	if (header_end == std::string::npos)
	{
		throw std::runtime_error("Incomplete HTTP header");
	}
	std::string header = buffer.substr(0, header_end + 4);
	if (!splitHeader(buffer, request))
		return false;
	if (!request.IsPOST)
		return true;
	if (request.headers.find("Content-Length") == request.headers.end())
		return true;
	size_t body_len = std::atol(request.headers["Content-Length"].c_str());
	size_t body_start = header_end + 4;
		if (buffer.size() < body_start + body_len)
	{
		throw std::runtime_error("Incomplete POST body");
	}
	request.body = buffer.substr(body_start, body_len);
	return true;
}

bool	httpPars::splitBody(std::string& result, HttpRequest& request)
{
	(void)result;
	(void) request;
	return true;
}

bool	httpPars::RespansePars(int fd, HttpRequest& request)
{
	(void)fd;
	(void)request;
	return true;
}