#pragma once

#include "../src/include.hpp"

struct HttpRequest;

class HttpResponse {
public:
    enum StatusCode {
        OK = 200,
        NOT_FOUND = 404,
        INTERNAL_SERVER_ERROR = 500,
        BAD_REQUEST = 400,
        METHOD_NOT_ALLOWED = 405,
        FOUND = 302
    };

    HttpResponse(StatusCode status = OK);
    void setStatusCode(StatusCode code);
    void setHeader(const std::string& name, const std::string& value);
    void setBody(const std::string& body);
    std::string toString() const;

private:
    StatusCode status_code;
    std::map<std::string, std::string> headers;
    std::string body;

    std::string getReasonPhrase(StatusCode code) const;
};

class RequestHandler {
public:
    static HttpResponse handleRequest(const HttpRequest& request);

private:
    static HttpResponse handleGetRequest(const HttpRequest& request);
    static HttpResponse handlePostRequest(const HttpRequest& request);
    static std::string sanitizePath(const std::string& path);
    static std::string getContentType(const std::string& file_path);
};

std::string generateResponse(const HttpRequest& request);