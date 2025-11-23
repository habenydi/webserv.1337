#include "../../include.hpp"

static std::string size_to_string(size_t n)
{
    std::ostringstream oss;
    oss << n;
    return oss.str();
}

HttpResponse::HttpResponse(StatusCode status) : status_code(status)
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

std::string HttpResponse::toString() const
{
    std::ostringstream response;

    response << "HTTP/1.1 " << status_code << " " << getReasonPhrase(status_code) << "\r\n"; // Changed from HTTP/1.0 to HTTP/1.1

    for (std::map<std::string, std::string>::const_iterator it = headers.begin();
         it != headers.end(); ++it)
    {
        response << it->first << ": " << it->second << "\r\n";
    }

    // Empty line to separate headers from body
    response << "\r\n";

    if (!body.empty())
        response << body;

    return response.str();
}

std::string HttpResponse::getReasonPhrase(StatusCode code) const
{
    switch (code)
    {
    case OK:
        return "OK";
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

HttpResponse RequestHandler::handleRequest(const HttpRequest &request)
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
    else
    {
        HttpResponse response(HttpResponse::METHOD_NOT_ALLOWED);
        response.setBody("<html><body><h1>405 Method Not Allowed</h1></body></html>");
        return response;
    }
}

std::string	getFileExtension(std::string file)
{
	size_t pos = file.find_last_of('.');
	return file.substr(pos);
}

HttpResponse RequestHandler::handleGetRequest(const HttpRequest &request)
{
    std::string safe_path = sanitizePath(request.path);
    // default is root
    if (safe_path == "/")
        safe_path = "/index.html";

    // Construct file path (assuming files are in a "www" directory)
    std::string file_path = "./www" + safe_path;

//	CGI cgi(config_file.root, request);
//	std::string ext = getFileExtension(safe_path);
//	if (interpreters.find(ext) != interpreters.end()) {
//	    cgi.run(interpreters[ext], file_path, NULL);
//	}

    std::ifstream file(file_path.c_str(), std::ios::binary);
    if (file.is_open())
    {
        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        file.close();

        HttpResponse response(HttpResponse::OK);

        std::string content_type = getContentType(file_path);
        response.setHeader("Content-Type", content_type);
        response.setBody(content);

        return response;
    }
    else
    {
        HttpResponse response(HttpResponse::NOT_FOUND);
        response.setBody("<html><body><h1>404 Not Found</h1><p>File: " + request.path + "</p></body></html>");
        return response;
    }
}

HttpResponse RequestHandler::handlePostRequest(const HttpRequest &request)
{
//	CGI cgi(config_file.root, request);
//	std::string ext = getFileExtension(safe_path);
//	if (interpreters.find(ext) != interpreters.end()) {
//	    cgi.run(interpreters[ext], file_path, request.body);
//	}
//
    // For now, just echo back the received data
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

// Remove any ".." to prevent directory traversal
// Replace any occurrences of ".." with empty string
// Ensure path starts with "/" and doesn't go above root
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

std::string RequestHandler::getContentType(const std::string &file_path)
{
    size_t dot_pos = file_path.find_last_of('.');
    if (dot_pos != std::string::npos)
    {
        std::string extension = file_path.substr(dot_pos + 1);

        if (extension == "html" || extension == "htm")
            return "text/html";
        else if (extension == "css")
            return "text/css";
        else if (extension == "js")
            return "application/javascript";
        else if (extension == "json")
            return "application/json";
        else if (extension == "png")
            return "image/png";
        else if (extension == "jpg" || extension == "jpeg")
            return "image/jpeg";
        else if (extension == "gif")
            return "image/gif";
        else if (extension == "txt")
            return "text/plain";
    }

    return "application/octet-stream"; // Default if no extension matched
}

std::string generateResponse(const HttpRequest &request)
{
    HttpResponse response = RequestHandler::handleRequest(request);
    return response.toString();
}
