#include "../../include.hpp"

bool	httpPars::splitHeader(std::string& result)
{
	if (result.find("\r\n\r\n") == std::string::npos)
		return false;
	
	std::vector <std::string> lines;
	size_t next , pos = 0;
	
	// Split by \r\n to get individual lines
	while ((next = result.find("\r\n", pos)) != std::string::npos)
	{
		std::string line = result.substr(pos, next - pos);
		if (!line.empty())  // Only add non-empty lines
			lines.push_back(line);
		pos = next + 2;
	}
	if (lines.empty())
		return false;
	// Parse the first line (request line): METHOD PATH VERSION
	std::istringstream first(lines[0]);
	first >> request.method >> request.path >> request.version;
	if (request.path.empty() || request.method.empty() || request.version.empty()) return false;
	if (request.version == "HTTP/1.1")
		request.version = "HTTP/1.0";
	if (request.method == "POST")
		request.IsPOST = true;
	
	// Parse headers (starting from line 1)
	for (size_t i = 1; i < lines.size(); i++)
	{
		size_t colon_pos = lines[i].find(":");
		if (colon_pos == std::string::npos)
			continue;
			
		std::string key = lines[i].substr(0, colon_pos);
		std::string value = lines[i].substr(colon_pos + 1);
		
		// Trim whitespace
		key.erase(0, key.find_first_not_of(" \t"));
		key.erase(key.find_last_not_of(" \t") + 1);
		value.erase(0, value.find_first_not_of(" \t"));
		value.erase(value.find_last_not_of(" \t") + 1);
		if (key == "Cookie")
		{
			StoreCookies(value);
			continue ;
		}
		request.headers[key] = value;
	}
	if (request.cookies.empty()
		|| request.cookies.find("session_id") == request.cookies.end()
		|| request.cookies["session_id"].empty())
	{
		std::stringstream ss;
		ss << "session_id";
		std::string	id = GenerateId();
		std::cout << id << std::endl;
		request.cookies[ss.str()] = id;
		sid[id].last_access = std::time(NULL);
		sid[id].logged_in = false;
		sid[id].email = "";
	}
	return true;
}

std::string GenerateId()
{
	static int counter;
	// Re-seed every time with time + counter for more randomness
	std::srand(std::time(NULL) + ++counter + std::clock());
	
	std::stringstream ss;
	ss << std::time(NULL) << std::rand() << std::rand() << counter;
	std::cout << "\n\n\n---->" << ss.str() << "\n"; 
	return ss.str();
}

void	httpPars::StoreCookies(std::string& line)
{
	size_t equal,start = 0, semicolon;
	while (start < line.size())
	{
		if ((equal = line.find("=", start)) == std::string::npos)
			break ;
		if ((semicolon = line.find(";", equal)) == std::string::npos)
			semicolon = line.size();
		std::string	key = line.substr(start, equal - start);
		std::string value = line.substr(equal + 1, semicolon - equal - 1);
		request.cookies[key] = value;
		std::cout << key << "<--->" << value << std::endl;
		start = semicolon + 1;
		while (start < line.size() && (line[start] == ' ' || line[start] == '\t'))
			start++;
	}
}

void httpPars::FindFilename(HttpRequest& request)
{
	size_t pos = request.body.find("filename=\"");
	std::string filename = "";

	if (pos != std::string::npos)
	{
		size_t start = pos + 10;
		size_t end = request.body.find("\"", start);
		if (end != std::string::npos)
			filename = request.body.substr(start, end - start);
	}

	// find content start
	size_t dataStart = request.body.find("\r\n\r\n");
	if (dataStart == std::string::npos)
		return;

	dataStart += 4;

	// find boundary end
	size_t dataEnd = request.body.rfind("\r\n------");
	if (dataEnd == std::string::npos)
		return;

	std::string fileData = request.body.substr(dataStart, dataEnd - dataStart);

	creat_and_write(filename, fileData);
}

bool	httpPars::RequestPars(std::string& buffer, globale& configue)
{
	request.cookies.clear();
	std::cout << buffer << std::endl;
	request.cookies["session_id"].clear();
	std::cout << buffer << std::endl;
	this->request.conf = configue;
	request.IsPOST = false;
	size_t header_end = buffer.find("\r\n\r\n");
	if (header_end == std::string::npos)
	{
		throw std::runtime_error("Incomplete HTTP header");
	}
	
	if (!splitHeader(buffer))
		return false;
	
	// For GET requests, generate response immediately
	if (!request.IsPOST)
	{
		response = generateResponse(request);
		return true;
	}
	
	// For POST requests, handle body
	StoreTheBody(buffer);
	FindFilename(request);
	response = generateResponse(request);
	return true;
}

void	httpPars::StoreTheBody(std::string& buffer)
{
	bool	is_chunked = false;
	if (request.headers.find("Transfer-Encoding") != request.headers.end())
	{
		std::string transfer = request.headers["Transfer-Encoding"];
		if (transfer == "chunked")
			is_chunked = true;
	}
	if (is_chunked)
		ChunkedBody(buffer);
	else
		RegularBody(buffer);
}

void	httpPars::ChunkedBody(std::string& buffer)
{
	size_t body_len = buffer.find("0\r\n");
	if (body_len == std::string::npos)
	{
		throw std::runtime_error("Incomplete POST body");
	}
	size_t header_end = buffer.find("\r\n\r\n");
	size_t body_start = header_end + 4;
	request.body = buffer.substr(body_start, body_len);
}

void	httpPars::RegularBody(std::string& buffer)
{
	size_t header_end = buffer.find("\r\n\r\n");
	if (header_end == std::string::npos)
		throw std::runtime_error("Incomplete POST body");
	if (request.headers.find("Content-Length") == request.headers.end())
		return;
	size_t body_len = std::atol(request.headers["Content-Length"].c_str());
	size_t body_start = header_end + 4;
	if (buffer.size() < body_start + body_len)
		throw std::runtime_error("Incomplete POST body");
	request.body = buffer.substr(body_start, body_len);
	buffer.erase(0, body_start + body_len);
}