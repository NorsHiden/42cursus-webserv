/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelidris <nelidris@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 18:44:36 by nelidris          #+#    #+#             */
/*   Updated: 2023/05/13 18:44:36 by nelidris         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __SERVER_HPP__
# define __SERVER_HPP__

# include <webserv.hpp>

class Server
{
	private:
		std::vector<ServerBlock> config; // imported from config file
		std::vector<std::pair<short, int> > server_sockets; // server_fds [PORT, SOCKET]
		std::map<short, std::list<Client> > clients; // [PORT, [CLIENTS]]
		
	public:
		/* Modifiers */
		bool empty(void) const { return (config.empty()); }
		
		/* ----------------PARSING---------------- */
		void importConfig(const std::string& filename);
		void getServer(std::ifstream& configFile);
		void listenComp(std::string line, ServerBlock& server);
		void importHost(std::string& host, ServerBlock& server);
		void serverNamesComp(std::string line, ServerBlock& server);
		void clientMaxBodySizeComp(std::string line, ServerBlock& server);
		void errorPagesComp(std::string line, ServerBlock& server);
		void locationComp(std::string line, std::ifstream& configFile, ServerBlock& server);
		void rootComp(std::string line, LocationBlock& location);
		void allowedMethodsComp(std::string line, LocationBlock& location);
		void indexComp(std::string line, LocationBlock& location);
		void cgiComp(std::string line, LocationBlock& location);
		void autoIndexComp(std::string line, LocationBlock& location);
		void uploadComp(std::string line, LocationBlock& location);
		void redirectionComp(std::string line, LocationBlock& location);
		friend std::ostream& operator<<(std::ostream& out, Server& server);
		
		/* ----------------BUILDING---------------- */
		void	buildServers(void);
		void	launchServers(void);
		void	checkNewConnections(fd_set& currentfds, fd_set& readfds);
		void	checkExistingConnections(fd_set& currentfds, fd_set& readfds, fd_set& writefds);


};

#endif /* __SERVER_HPP__ */