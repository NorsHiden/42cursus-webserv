#ifndef __SERVER_HPP__
# define __SERVER_HPP__

# include <webserv.hpp>

class Server
{
	private:
		std::vector<ServerBlock> config;
		
	public:
		// Parsing
		int	importConfig(const std::string filename);
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
};

#endif /* __SERVER_HPP__ */