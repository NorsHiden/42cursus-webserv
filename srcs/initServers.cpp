/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initServers.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelidris <nelidris@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 18:43:50 by nelidris          #+#    #+#             */
/*   Updated: 2023/06/08 14:45:13 by nelidris         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>

void	Server::checkNewConnections(fd_set& currentfds, fd_set& readfds)
{
	for (std::vector<std::pair<short, int> >::iterator it = server_sockets.begin(); it != server_sockets.end(); ++it)
	{
		if (FD_ISSET(it->second, &readfds))
		{
			// accept new incomming connection
			Client new_client;
			new_client.sock_fd = accept(it->second, (sockaddr*)&new_client.addr, &new_client.addr_len);
			if (new_client.sock_fd < 0)
				throw (std::runtime_error("accept failed"));
			// std::cout << "client: " << new_client.sock_fd << std::endl;
			if (fcntl(new_client.sock_fd, F_SETFL, O_NONBLOCK) == -1)
				throw (std::runtime_error("fcntl failed"));
			FD_SET(new_client.sock_fd, &currentfds);
			clients[it->first].push_back(new_client);
		}
	}
}

void	Server::checkExistingConnections(fd_set& currentfds, fd_set& readfds, fd_set& writefds)
{
	for (std::map<short, std::list<Client> >::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		std::list<Client>::iterator client = it->second.begin();
		while (client != it->second.end())
		{
			if (FD_ISSET(client->sock_fd, &readfds)) // ready to read
				client->readRequest();
			if (FD_ISSET(client->sock_fd, &writefds)) // ready to write
				client->handleRequest(it->first, config);
			if (client->action == REMOVE_CLIENT)
			{
				FD_CLR(client->sock_fd, &currentfds);
				close(client->sock_fd);
				if (client->cgi.pid > 0)
					kill(client->cgi.pid, SIGINT);
				it->second.erase(client++);
			}
			else
				++client;
		}
	}
}


void	Server::launchServers(void)
{
	fd_set currentfds, readfds, writefds;
	timeval time;

	FD_ZERO(&currentfds);
	memset(&time, 0, sizeof(time));
	for (size_t i = 0; i < server_sockets.size(); ++i)
		FD_SET(server_sockets[i].second, &currentfds);
	FD_CLR(0, &currentfds); // remove stdin from the fd_set;
	while (true)
	{
		readfds = writefds = currentfds;
		int  ret = select(FD_SETSIZE, &readfds, &writefds, NULL, &time);
		if (ret < 0)
			throw (std::runtime_error("select system call function failed."));
		if (!ret)
			continue ;
		checkNewConnections(currentfds, readfds);
		checkExistingConnections(currentfds, readfds, writefds);
	}
}


void	Server::buildServers(void)
{
	std::vector<std::pair<int, short> > hosted_ports;
	signal(SIGPIPE, SIG_IGN);
	std::cout << "\033[1;32m[*] Launching servers...\033[0m" << std::endl;
	for (size_t i = 0; i < config.size(); i++)
	{
		if (std::find(hosted_ports.begin(), hosted_ports.end(), config[i].listen) != hosted_ports.end())
			continue ;
		int sock = socket(AF_INET, SOCK_STREAM, 0); // creating a scoket
		if (sock < 0)
			throw (std::runtime_error("socket failed."));
		int reuse = 1;
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
			throw (std::runtime_error("setsockopt failed"));

		if (fcntl(sock, F_SETFL, O_NONBLOCK) < 0)
			throw (std::runtime_error("non-blocking failed."));
		// setting up the server address
		struct sockaddr_in serv_addr;
		memset(&serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = htonl(config[i].listen.first); // IP address in network byte order
		serv_addr.sin_port = htons(config[i].listen.second); // port in network byte order
		// binding the socket to the server address
		if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
			throw (std::runtime_error("bind failed due to invalid IP address or the port is already in use."));
		 // listening for incoming connections
		if (listen(sock, SOMAXCONN) < 0) // LISTEN_QUEUE is how many connections should be in the queue before starting to reject requests
			throw (std::runtime_error("listen failed due to permission denied."));
		hosted_ports.push_back(config[i].listen);
		std::pair<short, int> port_socket(config[i].listen.second, sock);
		server_sockets.push_back(port_socket);
		printReady(config[i]);
	}
}