/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelidris <nelidris@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 18:43:54 by nelidris          #+#    #+#             */
/*   Updated: 2023/05/13 18:43:54 by nelidris         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>

int main(int ac, char **av)
{
	Server server;
	if (ac != 2)
	{
		std::cerr << "Bad Arguments." << std::endl;
		return (1);
	}
	try
	{
		server.importConfig(av[1]);
		if (server.empty())
			throw (std::runtime_error("no server has been found."));
		server.buildServers();
		server.launchServers();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}