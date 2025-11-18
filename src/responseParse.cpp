#include "../headers/responseParse.hpp"

void responsePars::ReadFromFile(std::string &filename, std::string &content)
{
    std::ifstream input(filename.c_str());
    if (!input.is_open())
        throw std::runtime_error("Can't open the response file");

    std::string line;
    while (std::getline(input, line))
    {
        content += line;
        content += "\r\n";
    }
    input.close();
}

bool responsePars::ParseStatusLine(std::string &line, HttpResponse &response)
{
    std::istringstream iss(line);
    std::string version, code, message;

    iss >> version >> code;
    std::getline(iss >> std::ws, message);

    if (version.empty() || code.empty())
        return false;

    response.http_version = version;
    response.status_code = std::atoi(code.c_str());
    response.status_message = message;

    return true;
}

bool responsePars::ParseHeaders(std::vector<std::string> &lines, size_t &index, HttpResponse &response)
{
    while (index < lines.size())
    {
        std::string &line = lines[index];

        if (line.empty() || line == "\r" || line == "\r\n")
        {
            index++;
            break;
        }

        size_t colon_pos = line.find(':');
        if (colon_pos == std::string::npos)
        {
            index++;
            continue;
        }

        std::string key = line.substr(0, colon_pos);
        std::string value = line.substr(colon_pos + 1);

        key.erase(0, key.find_first_not_of(" \t\r\n"));
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);

        response.headers[key] = value;
        index++;
    }

    return true;
}

void responsePars::ParseBody(std::vector<std::string> &lines, size_t index, HttpResponse &response)
{
    for (; index < lines.size(); index++)
    {
        if (!response.body.empty())
            response.body += "\n";
        response.body += lines[index];
    }
}

void responsePars::ParseResponse(std::string &content, HttpResponse &result)
{
    std::vector<std::string> lines;
    std::istringstream stream(content);
    std::string line;

    while (std::getline(stream, line))
    {
        if (!line.empty() && line[line.length() - 1] == '\r')
            line = line.substr(0, line.length() - 1);
        lines.push_back(line);
    }

    if (lines.empty())
        throw std::runtime_error("Error: Empty response file");

    if (!ParseStatusLine(lines[0], result))
        throw std::runtime_error("Error: Invalid status line");

    size_t index = 1;
    if (!ParseHeaders(lines, index, result))
        throw std::runtime_error("Error: Failed to parse headers");

    ParseBody(lines, index, result);

    this->data = result;
}

void responsePars::parsing(std::string filename, HttpResponse &result)
{
    std::string content;
    ReadFromFile(filename, content);
    ParseResponse(content, result);
    result = this->data;
}