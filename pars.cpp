#include "pars.hpp"

void	pars::ReadFromFile(std::string& filename, std::string& content)
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
		content += line;
	}
	input.close();
}

std::vector<std::string>	pars::Tokenizer(std::string& content)
{
	std::vector <std::string> result;
	std::string token;
	int	count = 0;
	for (size_t i = 0; i < content.size(); i++)
	{
		char	c = content[i];
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

void pars::parsing(std::string filename, globale& result)
{
	std::string content;
	ReadFromFile(filename, content);
	std::vector<std::string> Tokens = Tokenizer(content);
	ParsTokens(Tokens);
	for (size_t i = 0; i < Tokens.size(); i++)
	{
		std::cout << '"' << Tokens[i] << '"' << std::endl;
	}
	result = this->data;
}

void	pars::ParseLocationBlock(std::vector<std::string>& token, size_t& index)
{
	LocationConfig loc;
    if (index + 1 >= token.size())
    {	throw std::runtime_error("Error: missing path after 'location'");}
	loc.path = token[++index];
	if (token[++index] != "{")
		throw std::runtime_error("Error: missing '{' after location path");
	index++;
	while (token[index] != "}")
	{
		if (token[index] == "root")
		{
			if (index + 1 >= token.size())
				throw std::runtime_error("Error: missing value after 'root'");
			loc.root = token[++index];
		}
		else if (token[index] == "autoindex")
		{
			std::cout << token[index] << std::endl;
			if (index + 1 >= token.size())
				throw std::runtime_error("Error: missing value after 'autoindex'");
			std::string ind = token[++index];
			if (ind == "on" || ind == "on;")
				loc.autoindex = true;
			else if (ind == "off")
				loc.autoindex = false;
			else
				throw std::runtime_error("Error: Invalid write in the 'autoindex'");
		}
		else if (token[index] != ";")
			throw std::runtime_error("Error : invalid type " + token[index]);
		index++;
		// for adding more location
	}
    if (token[index] != "}")
	{
        throw std::runtime_error("Error: missing closing '}' for location block");
	}
	data.location.push_back(loc);
}

void	pars::ParseServerBlock(std::vector<std::string>& token, size_t& index)
{
	index++;
	while (token[index] != "}")
	{
		if (token[index] == "listen")
			data.port.push_back(token[++index]);
		else if (token[index] == "root")
			data.root = token[++index];
		else if (token[index] == "index")
		{
			while (token[index] != ";")
				data.index.push_back(token[++index]);
		}
		else if (token[index] == "error_page")
		{
			int error_code = std::atoi(token[++index].c_str());
			std::string error_page = token[++index];
			data.error_page[error_code] = error_page;
		}
		else if (token[index] == "client_max_body_size")
			data.max_client_size = std::atol(token[++index].c_str());
		else if (token[index] == "location")
			ParseLocationBlock(token, index);
		else if (token[index] == "server_name")
			data.server_name = token[++index];
		else if (token[index] != ";")
			throw std::runtime_error("Unknow type " + token[index]);
		index++;
	}
	
}

void	pars::ParsTokens(std::vector<std::string> tokens)
{
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i] == "server")
		{
			if (tokens[++i] != "{")
				throw std::runtime_error ("Expect { after server");
			ParseServerBlock(tokens, i);
		}
		else
			throw std::runtime_error("Unknow Block\n" + tokens[i]);
	}
	
}