#include "../../include.hpp"
#include <iostream>
#include <ostream>

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
        response << "HTTP/1.1 " << status_code << " " << getReasonPhrase(status_code) << "\r\n";

        for (std::map<std::string, std::string>::const_iterator it = headers.begin();
             it != headers.end(); ++it)
        {
            response << it->first << ": " << it->second << "\r\n";
        }

        response << "\r\n";
    }
    else
        response << "HTTP/1.1 " << status_code << " " << getReasonPhrase(status_code) << "\r\n";

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

std::string getFileExtension(std::string file)
{
    size_t pos = file.find_last_of('.');
    if (pos == std::string::npos)
        return "";
    return file.substr(pos);
}

HttpResponse RequestHandler::handleGetRequest(const HttpRequest &request)
{
    std::string safe_path = sanitizePath(request.path);
    globale g = request.conf;

    std::string index = "/" + g.index;
    if (safe_path == "/")
        safe_path = index;

    std::string file_path = g.root + safe_path;
    std::string ext = getFileExtension(safe_path);

    // Check if this is a CGI script
    if (!ext.empty() && g.interpreters.find(ext) != g.interpreters.end())
    {
        size_t last_slash = safe_path.find_last_of('/');
        std::string filename;
        if (last_slash != std::string::npos)
            filename = safe_path.substr(last_slash + 1);
        else
            filename = safe_path;

        CGI cgi("./www/cgi-bin", request);
        cgi.run(g.interpreters[ext], filename, "");

        HttpResponse response(HttpResponse::OK);

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
        std::string not_found_path = g.root + "/404.html";
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
}

HttpResponse RequestHandler::handlePostRequest(const HttpRequest &request)
{
    std::string safe_path = sanitizePath(request.path);
    globale g = request.conf;

    if (safe_path == "/")
        safe_path = "/index.html";

    std::string file_path = g.root + safe_path;
    std::string ext = getFileExtension(safe_path);

    if (!ext.empty() && g.interpreters.find(ext) != g.interpreters.end())
    {
        size_t last_slash = safe_path.find_last_of('/');
        std::string filename;
        if (last_slash != std::string::npos)
            filename = safe_path.substr(last_slash + 1);
        else
            filename = safe_path;

        CGI cgi("./www/cgi-bin", request);
        cgi.run(g.interpreters[ext], filename, request.body);

        HttpResponse response(HttpResponse::OK);

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

std::string RequestHandler::getContentType(const std::string &file_path)
{
    size_t dot_pos = file_path.find_last_of('.');
    if (dot_pos == std::string::npos)
        return "application/octet-stream";

    std::string ext = file_path.substr(dot_pos + 1);

    static std::map<std::string, std::string> mimeTypes;

    if (mimeTypes.empty())
    {
        mimeTypes["html"] = "text/html";
        mimeTypes["htm"] = "text/html";
        mimeTypes["css"] = "text/css";
        mimeTypes["js"] = "application/javascript";
        mimeTypes["json"] = "application/json";
        mimeTypes["png"] = "image/png";
        mimeTypes["jpg"] = "image/jpeg";
        mimeTypes["jpeg"] = "image/jpeg";
        mimeTypes["gif"] = "image/gif";
        mimeTypes["txt"] = "text/plain";
    }

    std::map<std::string, std::string>::iterator it = mimeTypes.find(ext);
    if (it != mimeTypes.end())
        return it->second;

    return "application/octet-stream";
}

std::string generateResponse(const HttpRequest &request)
{
    HttpResponse response = RequestHandler::handleRequest(request);
    return response.toString();
}
