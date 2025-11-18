#pragma once

#include "../src/include.hpp"

class HttpResponse
{
public:
    std::string  http_version;
    int          status_code;
    std::string  status_message;
    std::map<std::string, std::string> headers;
    std::string  body;

    HttpResponse()
        : http_version(), status_code(0), status_message(),
          headers(), body()
    {
    }
};

class responsePars
{
private:
    HttpResponse data;
    
public:
    void ReadFromFile(std::string& filename, std::string& content);
    void ParseResponse(std::string& content, HttpResponse& result);
    bool ParseStatusLine(std::string& line, HttpResponse& response);
    bool ParseHeaders(std::vector<std::string>& lines, size_t& index, HttpResponse& response);
    void ParseBody(std::vector<std::string>& lines, size_t index, HttpResponse& response);
    void parsing(std::string filename, HttpResponse& result);
};