/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pars.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassir <yassir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 09:25:39 by yassir            #+#    #+#             */
/*   Updated: 2025/11/09 11:17:01 by yassir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
};