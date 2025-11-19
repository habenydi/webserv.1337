#include "include.hpp"

void pars::ReadFromFile(std::string &filename, std::string &content)
{
	std::ifstream input(filename.c_str());
	if (!input.is_open())
		throw std::runtime_error("Can't open the conf file");
	std::string line;
	while (std::getline(input, line))
	{
		size_t pos = line.find('#');
		if (pos != std::string::npos)
			line = line.substr(0, pos);
		// keep a separator so tokens from adjacent lines don't merge
		content += line;
		content += '\n';
	}
	input.close();
}

std::vector<std::string> pars::Tokenizer(std::string &content)
{
	std::vector<std::string> result;
	std::string token;
	int count = 0;
	for (size_t i = 0; i < content.size(); i++)
	{
		char c = content[i];
		if (isspace(c))
		{
			if (!token.empty())
			{
				result.push_back(token);
				token.clear();
			}
		}
		else if (c == '{' || c == '}' || c == ';')
		{
			if (c == '{')
				count++;
			else if (c == '}')
			{
				if (!count)
					throw std::runtime_error("the '}' shoud be after the open one");
				count--;
			}
			if (!token.empty())
			{
				result.push_back(token);
				token.clear();
			}
			result.push_back(std::string(1, c));
		}
		else
			token += c;
	}
	if (count)
		throw std::runtime_error("the operators should be closed");
	if (!token.empty())
		result.push_back(token);
	return (result);
}

void pars::parsing(std::string filename, std::vector<globale>& result)
{
	std::string content;
	Index = 0;
	ReadFromFile(filename, content);
	std::vector<std::string> Tokens = Tokenizer(content);
	ParsTokens(Tokens);
	for (size_t i = 0; i < Tokens.size(); i++)
	{
		std::cout << '"' << Tokens[i] << '"' << std::endl;
	}
	result = this->data;
}

void pars::ParseLocationBlock(std::vector<std::string> &token, size_t &index)
{
	LocationConfig loc;
	if (index + 1 >= token.size())
		throw std::runtime_error("Error: missing path after 'location'");
	loc.path = token[++index];
	if (index + 1 >= token.size() || token[++index] != "{")
		throw std::runtime_error("Error: missing '{' after location path");
	index++; // move to first token inside location
	while (index < token.size() && token[index] != "}")
	{
		if (token[index] == "root")
		{
			if (index + 1 >= token.size())
				throw std::runtime_error("Error: missing value after 'root'");
			loc.root = token[++index];
		}
		else if (token[index] == "index")
		{
			++index;
			if (index >= token.size())
				throw std::runtime_error("Error: missing values after 'index'");
			while (index < token.size() && token[index] != ";")
			{
				loc.index.push_back(token[index]);
				++index;
			}
			if (index >= token.size() || token[index] != ";")
				throw std::runtime_error("Error: missing ';' after 'index' in location");
		}
		else if (token[index] == "allow_methods")
		{
			++index;
			if (index >= token.size())
				throw std::runtime_error("Error: missing values after 'allow_methods'");
			while (index < token.size() && token[index] != ";")
			{
				loc.allowed_methods.push_back(token[index]);
				++index;
			}
			if (index >= token.size() || token[index] != ";")
				throw std::runtime_error("Error: missing ';' after 'allow_methods' in location");
		}
		else if (token[index] == "upload_store")
		{
			if (index + 1 >= token.size())
				throw std::runtime_error("Error: missing value after 'upload_store'");
			loc.upload_store = token[++index];
		}
		else if (token[index] == "cgi_pass")
		{
			if (index + 1 >= token.size())
				throw std::runtime_error("Error: missing value after 'cgi_pass'");
			loc.cgi_pass = token[++index];
		}
		else if (token[index] == "autoindex")
		{
			if (index + 1 >= token.size())
				throw std::runtime_error("Error: missing value after 'autoindex'");
			std::string ind = token[++index];
			if (ind == "on")
				loc.autoindex = true;
			else if (ind == "off")
				loc.autoindex = false;
			else
				throw std::runtime_error("Error: Invalid value for 'autoindex'");
		}
		else if (token[index] != ";")
			throw std::runtime_error("Error : invalid type " + token[index]);
		index++;
	}
	if (index >= token.size() || token[index] != "}")
		throw std::runtime_error("Error: missing closing '}' for location block");
	data[Index].location.push_back(loc);
}

void pars::ParseServerBlock(std::vector<std::string> &token, size_t &index)
{
	index++;
	while (index < token.size() && token[index] != "}")
	{
		if (token[index] == "listen")
		{
			if (index + 1 >= token.size())
				throw std::runtime_error("Error: missing value after 'listen'");
			data[Index].port.push_back(token[++index]);
		}
		else if (token[index] == "host")
		{
			if (index + 1 >= token.size())
				throw std::runtime_error("Error: missing value after 'host'");
			data[Index].host = token[++index];
		}
		else if (token[index] == "root")
		{
			if (index + 1 >= token.size())
				throw std::runtime_error("Error: missing value after 'root'");
			data[Index].root = token[++index];
		}
		else if (token[index] == "index")
		{
			++index;
			if (index >= token.size())
				throw std::runtime_error("Error: missing values after 'index'");
			while (index < token.size() && token[index] != ";")
			{
				data[Index].index.push_back(token[index]);
				++index;
			}
			if (index >= token.size() || token[index] != ";")
				throw std::runtime_error("Error: missing ';' after 'index'");
		}
		else if (token[index] == "error_page")
		{
			if (index + 2 >= token.size())
				throw std::runtime_error("Error: invalid error_page directive");
			int error_code = std::atoi(token[++index].c_str());
			std::string error_page = token[++index];
			data[Index].error_page[error_code] = error_page;
		}
		else if (token[index] == "client_max_body_size")
		{
			if (index + 1 >= token.size())
				throw std::runtime_error("Error: missing value for client_max_body_size");
			data[Index].max_client_size = std::atol(token[++index].c_str());
		}
		else if (token[index] == "location")
			ParseLocationBlock(token, index);
		else if (token[index] == "server_name")
		{
			if (index + 1 >= token.size())
				throw std::runtime_error("Error: missing value after 'server_name'");
			data[Index].server_name = token[++index];
		}
		else if (token[index] != ";")
			throw std::runtime_error("Unknown directive: " + token[index]);
		++index;
	}
}

void pars::ParsTokens(std::vector<std::string> tokens)
{
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i] == "server")
		{
			if (tokens[++i] != "{")
				throw std::runtime_error("Expect { after server");
			data.push_back(globale());	
			ParseServerBlock(tokens, i);
			Index++;
		}
		else
			throw std::runtime_error("Unknow Block\n" + tokens[i]);
	}
}