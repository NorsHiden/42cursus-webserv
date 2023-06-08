/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelidris <nelidris@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 18:44:37 by nelidris          #+#    #+#             */
/*   Updated: 2023/06/08 14:00:54 by nelidris         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVERBLOCK_HPP__
# define __SERVERBLOCK_HPP__

# include <webserv.hpp>

struct ServerBlock
{
	std::pair<int, short> listen; // [ IP_ADDR, PORT ]
	std::vector<std::string> server_names;
	ssize_t client_max_body_size; // IN BYTES ONLY
	std::map<short, std::string> error_pages; // [ PATH, STATUS_CODE ]
	std::map<std::string, LocationBlock> locations; // [ URI, LOCATION_BLOCK ]

	ServerBlock(): client_max_body_size(0) {}
};

#endif /* __SERVERBLOCK_HPP__ */