#pragma once

#include "../../external.hpp"
#include "../../Helper/globale.hpp"

class pars
{
	private:
		std::vector<globale> data;
	public:
		int	Index;
		void	ReadFromFile(std::string& filename, std::string& content);
		std::vector<std::string>	Tokenizer(std::string& content);
		void	ParsTokens(std::vector<std::string>  tokens);
		void 	parsing(std::string filename, std::vector<globale>& result);
		void	ParseServerBlock(std::vector <std::string>& token, size_t& index);
		void	ParseLocationBlock(std::vector<std::string>& token, size_t& index);
};