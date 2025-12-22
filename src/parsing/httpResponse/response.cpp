#include "../../include.hpp"
#include <iostream>
#include <ostream>

static std::string size_to_string(size_t n)
{
	std::ostringstream oss;
	oss << n;
	return oss.str();
}

HttpResponse::HttpResponse(StatusCode status) : file_fd(-1), status_code(status)
{
	headers["Content-Type"] = "text/html";
	headers["Connection"] = "close";
}

void HttpResponse::setStatusCode(StatusCode code)
{
	status_code = code;
}

void HttpResponse::setHeader(const std::string &name, const std::string &value)
{
	headers[name] = value;
}

void HttpResponse::setBody(const std::string &body)
{
	this->body = body;
	headers["Content-Length"] = size_to_string(body.length());
}

void HttpResponse::clearHeaders()
{
	headers.clear();
}

// Add new method to set body without Content-Length
void HttpResponse::setRawBody(const std::string &body)
{
	this->body = body;
}

// Only add HTTP status line if we have headers
// (CGI responses with their own headers don't need it duplicated)
std::string HttpResponse::toString() const
{
	std::ostringstream response;
	if (!headers.empty())
	{
		response << "HTTP/1.0 " << status_code << " " << getReasonPhrase(status_code) << "\r\n";

		for (std::map<std::string, std::string>::const_iterator it = headers.begin();
			 it != headers.end(); ++it)
		{
			response << it->first << ": " << it->second << "\r\n";
		}

		response << "\r\n";
	}
	else
		response << "HTTP/1.0 " << status_code << " " << getReasonPhrase(status_code) << "\r\n";

	if (!body.empty())
		response << body;

	return response.str();
}

// ADD THIS NEW METHOD
std::string HttpResponse::toStringHeadersOnly(HttpRequest& request) const
{
	std::ostringstream response;
	response << "HTTP/1.0 " << status_code << " " << getReasonPhrase(status_code) << "\r\n";

	for (std::map<std::string, std::string>::const_iterator it = headers.begin();
		 it != headers.end(); ++it)
	{
		response << it->first << ": " << it->second << "\r\n";
	}
	for (std::map<std::string, std::string>::const_iterator it = request.cookies.begin() ; it != request.cookies.end(); it++)
	{
		response << "Set-Cookie: " << it->first << "=" << it->second << "\r\n";
	}
	response << "\r\n";
	return response.str();
}

std::string HttpResponse::getReasonPhrase(StatusCode code) const
{
	switch (code)
	{
	case OK:
		return "OK";
	case FORBIDDEN:
		return "Forbidden";
	case NOT_FOUND:
		return "Not Found";
	case INTERNAL_SERVER_ERROR:
		return "Internal Server Error";
	case BAD_REQUEST:
		return "Bad Request";
	case METHOD_NOT_ALLOWED:
		return "Method Not Allowed";
	case FOUND:
		return "Found";
	default:
		return "Unknown";
	}
}

HttpResponse RequestHandler::handleDeleteRequest(HttpRequest &request)
{
	std::string safe_path = sanitizePath(request.path);
	globale g = request.conf;

	// Only allow files inside /uploads
	std::string uploads_dir = g.root + "/uploads";
	std::string file_path = g.root + safe_path;

	if (file_path.find(uploads_dir) != 0) // check path starts with uploads_dir
	{
		HttpResponse response(HttpResponse::FORBIDDEN);
		response.setBody("<html><body><h1>403 Forbidden</h1></body></html>");
		return response;
	}

	if (std::remove(file_path.c_str()) == 0)
	{
		HttpResponse response(HttpResponse::OK);
		response.setBody("<html><body><h1>200 OK - File Deleted</h1></body></html>");
		return response;
	}
	else
	{
		HttpResponse response(HttpResponse::NOT_FOUND);
		response.setBody("<html><body><h1>404 Not Found - File Not Found</h1></body></html>");
		return response;
	}
}

HttpResponse RequestHandler::handleRequest(HttpRequest &request)
{
	if (request.version != "HTTP/1.0")
	{
		HttpResponse response(HttpResponse::BAD_REQUEST);
		response.setBody("<html><body><h1>400 Bad Request - Invalid HTTP version</h1></body></html>");
		return response;
	}
	if (request.method == "GET")
		return handleGetRequest(request);
	else if (request.method == "POST")
		return handlePostRequest(request);
	else if (request.method == "DELETE")
		return handleDeleteRequest(request);
	else
	{
		HttpResponse response(HttpResponse::METHOD_NOT_ALLOWED);
		response.setBody("<html><body><h1>405 Method Not Allowed</h1></body></html>");
		return response;
	}
}

std::string getFileExtension(std::string file)
{
	size_t pos = file.find_last_of('.');
	if (pos == std::string::npos)
		return "";
	return file.substr(pos);
}

// Handle CGI script execution
HttpResponse RequestHandler::handleCGI(HttpRequest &request, const std::string &safe_path, const std::string &ext, globale &g)
{
	size_t last_slash = safe_path.find_last_of('/');
	std::string filename;
	if (last_slash != std::string::npos)
		filename = safe_path.substr(last_slash + 1);
	else
		filename = safe_path;

	// Execute CGI script
	CGI cgi("./www/cgi-bin", request);
	cgi.run(g.interpreters[ext], filename, request.body);

	// Create response
	HttpResponse response(HttpResponse::OK);

	// Check if CGI output includes its own headers
	if (cgi.output.find("Content-Type:") != std::string::npos)
	{
		response.clearHeaders();
		response.setRawBody(cgi.output);
	}
	else
	{
		response.setBody(cgi.output);
	}

	return response;
}

HttpResponse RequestHandler::handleGetRequest(HttpRequest &request)
{
	std::cout << "\n\n\n\n\n\n\n\n " << request.path << "\n\n\n\n\n\n\n" << sid[request.cookies["session_id"]].logged_in;
	if (request.path == "/html/login.html" && sid[request.cookies["session_id"]].logged_in)
	{
		request.path = "/html/profile.html";
		return handleGetRequest(request);
	}
	std::string safe_path = sanitizePath(request.path);
	globale g = request.conf;
	std::string index = "/" + g.index;
	std::string file_path = "";
	std::map<std::string, std::string>::const_iterator it = request.conf.redirection.begin();
	for (; it != request.conf.redirection.end(); it++)
    {
        if (request.path == it->first)
        {
            HttpResponse response(HttpResponse::FOUND);
            response.setHeader("Location", it->second);
            return response;
        }
    }
	for (size_t i = 0; i < request.conf.location.size(); i++)
	{
		if (safe_path == request.conf.location[i].path)
		{
			if (!request.conf.location[i].index.empty())
				index = request.conf.location[i].index;
			safe_path = index;
			file_path = g.location[i].root + safe_path;
		}
	}
	if (file_path.empty())
	{
		if (safe_path == "/")
			safe_path = index;
		file_path = g.root + safe_path;
	}
	std::string ext = getFileExtension(safe_path);

	// Check if this is a CGI script
	if (!ext.empty() && g.interpreters.find(ext) != g.interpreters.end())
		return handleCGI(request, safe_path, ext, g);

	int fd = open(file_path.c_str(), O_RDONLY);
	if (fd != -1)
	{
		struct stat file_stat;
		if (fstat(fd, &file_stat) == 0)
		{
			HttpResponse response(HttpResponse::OK);
			std::string content_type = getContentType(file_path, g);
			response.setHeader("Content-Type", content_type);
			response.setHeader("Content-Length", size_to_string(file_stat.st_size));

			response.file_fd = fd;
			response.file_size = file_stat.st_size;
			return response;
		}
		close(fd);
	}

	// 404 error handling
	std::string not_found_path = g.root + g.error_page[404];
	std::ifstream error_file(not_found_path.c_str(), std::ios::binary);
	HttpResponse response(HttpResponse::NOT_FOUND);

	if (error_file.is_open())
	{
		std::string error_content((std::istreambuf_iterator<char>(error_file)),
								  std::istreambuf_iterator<char>());
		error_file.close();
		response.setHeader("Content-Type", "text/html");
		response.setBody(error_content);
	}
	else
	{
		response.setBody("<html><body><h1>404 Not Found</h1><p>File: " + request.path + "</p></body></html>");
	}

	return response;
}

HttpResponse	RequestHandler::HandleCookieFile(HttpRequest& request)
{
	size_t pos = request.body.find("email=");
	if (pos != std::string::npos)
	{
		size_t end = request.body.find("&", pos);
		if (end == std::string::npos)
            end = request.body.size();
		sid[request.cookies["session_id"]].email
			= request.body.substr(pos + 6, end - (pos + 6));
		sid[request.cookies["session_id"]].logged_in = true;
		sid[request.cookies["session_id"]].last_access
			= sid[request.cookies["session_id"]].last_access - std::atol(GenerateId().c_str());
	}
	HttpResponse res(HttpResponse::FOUND);
	res.setHeader("Location", "/html/profile.html");
	std::cout << "\n\n\n\n\n\n\n\n ====>" << request.path << "\n\n\n\n\n\n\n2======>" << sid[request.cookies["session_id"]].logged_in;
	return res;
}

HttpResponse RequestHandler::handlePostRequest(HttpRequest &request)
{
	std::string safe_path = sanitizePath(request.path);
	globale g = request.conf;

	if (request.path == "/html/profile.html")
		return HandleCookieFile(request);
	if (safe_path == "/")
		safe_path = "/index.html";

	std::string file_path = g.root + safe_path;
	std::string ext = getFileExtension(safe_path);

	// Check if this is a CGI script
	if (!ext.empty() && g.interpreters.find(ext) != g.interpreters.end())
		return handleCGI(request, safe_path, ext, g);

	HttpResponse response(HttpResponse::OK);
	std::string response_body = "<html><body>"
								"<h1>POST Request Received</h1>"
								"<p>Path: " +
								request.path + "</p>"
											   "<p>Body: " +
								request.body + "</p>"
											   "</body></html>";
	response.setBody(response_body);
	return response;
}

// Sanitize the path to prevent directory traversal attacks
// remove any ".." segments and ensure it starts with "/"
std::string RequestHandler::sanitizePath(const std::string &path)
{
	std::string sanitized = path;

	size_t pos = 0;
	while ((pos = sanitized.find("..", pos)) != std::string::npos)
		sanitized.erase(pos, 2);

	if (sanitized.empty() || sanitized[0] != '/')
		sanitized = "/" + sanitized;

	return sanitized;
}

std::string RequestHandler::getContentType(const std::string &file_path, globale &conf)
{
	size_t dot_pos = file_path.find_last_of('.');
	if (dot_pos == std::string::npos)
		return "application/octet-stream";

	std::string ext = file_path.substr(dot_pos + 1);

	std::map<std::string, std::string>::iterator it = conf.mimetype.find(ext);
	if (it != conf.mimetype.end())
		return it->second;

	return "application/octet-stream";
}

Response generateResponse(HttpRequest &request)
{
	Response res;
	HttpResponse response = RequestHandler::handleRequest(request);

	// If response has a file descriptor, return only headers
	if (response.file_fd != -1)
	{
		res.header = response.toStringHeadersOnly(request);
		std::cout << "\n\n-->  " << res.header << std::endl;
		res.fd = response.file_fd;
		res.size = response.file_size;
	}
	else
	{
		res.header = response.toString();
		res.fd = -1;
		res.size = 0;
	}

	return res;
}
