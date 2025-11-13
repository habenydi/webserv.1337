#pragma once

#include "include.hpp"
#include "globale.hpp"

class pars
{
	private:
		globale data;
	public:
		void	ReadFromFile(std::string& filename, std::string& content);
		std::vector<std::string>	Tokenizer(std::string& content);
		void 	parsing(std::string filename, globale& result);
		void	ParsTokens(std::vector<std::string>  tokens);
		void	ParseServerBlock(std::vector <std::string>& token, size_t& index);
		void	ParseLocationBlock(std::vector<std::string>& token, size_t& index);
};