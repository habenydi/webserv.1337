/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   globale.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yassir <yassir@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/09 09:55:20 by yassir            #+#    #+#             */
/*   Updated: 2025/11/09 11:27:08 by yassir           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "include.hpp"

class globale
{
	private:
		int	port;
		std::string path;
		std::string	root;
		bool autoindex;
		std::vector<std::string> index;
		std::map<int, std::string> error_page;
	public:
		globale& operator=(const globale& other)
		{
			if (this != &other)
			{
				this->path = other.path;
				this->port = other.port;
				this->root = other.root;
				this->autoindex = other.autoindex;
				this->index = other.index;
				this->error_page = other.error_page;
				// if you add any thing int the private of class you have to add here
			}
			return (*this);
		}
};

