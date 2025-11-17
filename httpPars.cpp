// #include "httpPars.hpp"

// bool	httpPars::splitHeader(std::string& result, HttpRequest& request)
// {
// 	if (result.find("\r\n\r\n") == std::string::npos)
// 		return false;
// 	std::vector <std::string> lines;
// 	size_t next , pos;
// 	while ((next = result.find("\r\n", pos)) != std::string::npos)
// 	{
// 		lines.push_back(result.substr(pos, next - pos));
// 		pos = next + 2;
// 	}
// 	if (pos < result.size())
// 		lines.push_back(result.substr(pos));
// 	if (lines.empty())
// 		return false;
// 	std::istringstream first(lines[0].c_str());
// 	first >> request.method >> request.path >> request.version;
// 	if (request.path.empty() || request.method.empty() || request.version.empty())
// 		return false;
// 	if (request.method == "POST")
// 		request.IsPOST = true;
// 	for (size_t i = 1; i < lines.size(); i++)
// 	{
// 		if ((pos = lines[i].find(":")) == std::string::npos)
// 			continue;
// 		std::string key = lines[i].substr(0, pos);
// 		std::string value = lines[i].substr(pos + 1);
// 		key.erase(0, key.find_first_not_of(" \t"));
// 		key.erase(key.find_last_not_of(" \t") + 1);
// 		value.erase(0, value.find_first_not_of(" \t"));
// 		value.erase(value.find_last_not_of(" \t") + 1);
// 		request.headers[key] = value;
// 	}
// 	std::map<std::string, std::string>::iterator it;
// 	for (it = request.headers.begin(); it != request.headers.end(); ++it)
// 	{
// 		std::cout << '"' << it->first << '"' 
// 				<< " <--> "
// 				<< '"' << it->second << '"' 
// 				<< std::endl;
// 	}
// 	return true;
	
// }
// bool	httpPars::RequestPars(int fd, HttpRequest& request)
// {
// 	request.IsPOST = false;
// 	char buf[1024];
// 	int bytes;
// 	std::string result;
// 	while ((bytes = read(fd, &buf, sizeof(buf))))
// 	{
// 		result.append(buf, bytes);
// 		if (result.size() > MAX_REQUEST_SIZE)
// 		{
// 			// error too large request
// 			return false;
// 		}
// 		if (result.find("\r\n\r\n") != std::string::npos)
// 			break ;
// 	}
// 	if (bytes == -1)
// 	{
// 		std::cerr << "read\n";
// 		return (false);
// 	}
// 	if (!splitHeader(result, request))
// 		return false;
// 	if (request.IsPOST)
// 		return (splitBody(result, request, fd));
// 	return true;
// }

// bool	httpPars::splitBody(std::string& result, HttpRequest& request, int& fd)
// {
// 	std::cout << "\n\n\n in here\n\n";
// 	size_t is_chunked = 0, contentlenght = 0;
// 	if (request.headers.count("Content-Length"))
// 		contentlenght = std::atoi(request.headers["Content-Length"].c_str());
// 	if (contentlenght)
// 	{
// 		char buf[contentlenght];
// 		size_t bytes = read(fd, &buf, contentlenght);
// 		request.body.append(buf);
// 	}
// 	return true;
// }

// bool	httpPars::RespansePars(int fd, HttpRequest& request)
// {
// 	(void)fd;
// 	(void)request;
// 	return true;
// }