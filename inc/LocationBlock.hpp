/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlock.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelidris <nelidris@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 18:44:27 by nelidris          #+#    #+#             */
/*   Updated: 2023/05/13 18:44:28 by nelidris         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __LOCATIONBLOCK_HPP__
# define __LOCATIONBLOCK_HPP__

# include <webserv.hpp>

struct LocationBlock
{
	std::string root;
	std::set<std::string> allowed_methods; // [ GET, POST, DELETE ] ONLY
	std::vector<std::string> index;
	std::pair<std::string, std::string> cgi; // [ PATH, EXTENSION ]
	bool autoindex;
	std::string upload; // PATH
	std::pair<std::string, short> redirection; // [ URI, STATUS_CODE ]
	LocationBlock(): autoindex(false) {}
};

#endif /* __LOCATIONBLOCK_HPP__ */