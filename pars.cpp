/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pars.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassir <yassir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 11:02:36 by yassir            #+#    #+#             */
/*   Updated: 2025/11/09 11:17:33 by yassir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
	(void) result;
	ReadFromFile(filename, content);
	std::vector<std::string> Tokens = Tokenizer(content);
	ParsTokens(Tokens);
	for (size_t i = 0; i < Tokens.size(); i++)
	{
		std::cout << '"' << Tokens[i] << '"' << std::endl;
	}
	result = this->data;
}

void	pars::ParsTokens(std::vector<std::string> tokens)
{
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i] == "server")
		{
			if (tokens[i + 1] != "{")
				throw std::runtime_error ("Expect { after server");
			// pars the server hoop here
		}
	}
	
}