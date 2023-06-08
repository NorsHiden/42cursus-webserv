/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelidris <nelidris@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 18:43:54 by nelidris          #+#    #+#             */
/*   Updated: 2023/06/08 14:27:44 by nelidris         ###   ########.fr       */
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
		// importing config file in green to make it more visible
		std::cout << "\033[1;32m[*] Importing config file...\033[0m" << std::endl;
		server.importConfig(av[1]);
		if (server.empty())
			throw (std::runtime_error("no server has been found."));
		std::cout << "\033[1;32m[*] Building servers...\033[0m" << std::endl;
		server.buildServers();
		server.launchServers();
	}
	catch (const std::exception& e)
	{
		// error in red
		std::cerr << "\033[1;31m[ERROR] " << e.what() << "\033[0m" << std::endl;
	}
	return (0);
}