#pragma once

#include "../../../src/include.hpp"

struct HttpRequest;
struct Response;

class HttpResponse
{
public:
    enum StatusCode
    {
        OK = 200,
        NOT_FOUND = 404,
        INTERNAL_SERVER_ERROR = 500,
        BAD_REQUEST = 400,
        METHOD_NOT_ALLOWED = 405,
        FOUND = 302,
        FORBIDDEN = 403
    };

    HttpResponse(StatusCode status = OK);
    void setStatusCode(StatusCode code);
    void setHeader(const std::string &name, const std::string &value);
    void setBody(const std::string &body);
    void clearHeaders();
    void setRawBody(const std::string &body);
    std::string toString() const;
    std::string toStringHeadersOnly() const;

    int file_fd;

private:
    StatusCode status_code;
    std::map<std::string, std::string> headers;
    std::string body;

    std::string getReasonPhrase(StatusCode code) const;
};

class RequestHandler
{
public:
    static HttpResponse handleRequest(const HttpRequest &request);

private:
    static HttpResponse handleGetRequest(const HttpRequest &request);
    static HttpResponse handlePostRequest(const HttpRequest &request);
    static std::string sanitizePath(const std::string &path);
    static std::string getContentType(const std::string &file_path, globale &conf);
    static HttpResponse handleCGI(const HttpRequest &request, const std::string &safe_path, const std::string &ext, globale &g);
    static HttpResponse handleDeleteRequest(const HttpRequest &request);
};

Response generateResponse(const HttpRequest &request);