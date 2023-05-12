#include <webserv.hpp>

void	Server::checkNewConnections(fd_set& currentfds, fd_set& readfds)
{
	for (size_t i = 0; i < server_sockets.size(); ++i)
	{
		if (FD_ISSET(server_sockets[i], &readfds))
		{
			// accept new incomming connection
			Client new_client;
			new_client.sock_fd = accept(server_sockets[i], (sockaddr*)&new_client.addr, &new_client.addr_len);
			if (new_client.sock_fd < 0)
				throw (std::runtime_error("accept failed"));
			// std::cout << "client: " << new_client.sock_fd << std::endl;
			if (fcntl(new_client.sock_fd, F_SETFL, O_NONBLOCK) == -1)
				throw (std::runtime_error("fcntl failed"));
			FD_SET(new_client.sock_fd, &currentfds);
			clients.push_back(new_client);
		}
	}
}

void	Server::checkExistingConnections(fd_set& currentfds, fd_set& readfds, fd_set& writefds)
{
	std::list<Client>::iterator client = clients.begin();
	while (client != clients.end())
	{
		if (FD_ISSET(client->sock_fd, &readfds)) // ready to read
			client->readRequest();
		if (FD_ISSET(client->sock_fd, &writefds)) // ready to write
			client->handleRequest(config);
		if (client->action == REMOVE_CLIENT)
		{
			FD_CLR(client->sock_fd, &currentfds);
			close(client->sock_fd);
			clients.erase(client++);
		}
		else
			++client;
	}
}


void	Server::launchServers(void)
{
	fd_set currentfds, readfds, writefds;
	timeval time;

	FD_ZERO(&currentfds);
	memset(&time, 0, sizeof(time));
	for (size_t i = 0; i < server_sockets.size(); ++i)
		FD_SET(server_sockets[i], &currentfds);
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
	for (size_t i = 0; i < config.size(); i++)
	{
		int sock = socket(AF_INET, SOCK_STREAM, 0); // creating a scoket
		if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1)
			throw (std::runtime_error("non-blocking failed."));
		if (!sock)
			throw (std::runtime_error("socket failed."));
		// setting up the server address
		struct sockaddr_in serv_addr;
		memset(&serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_addr.s_addr = htonl(config[i].listen.first); // IP address in network byte order
		serv_addr.sin_port = htons(config[i].listen.second); // port in network byte order
		// binding the socket to the server address
		if (bind(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
			throw (std::runtime_error("bind failed due to the port is already in use."));
		 // listening for incoming connections
		if (listen(sock, LISTEN_QUEUE) < 0) // LISTEN_QUEUE is how many connections should be in the queue before starting to reject requests
			throw (std::runtime_error("listen failed due to permission denied."));
		server_sockets.push_back(sock);
	}
}