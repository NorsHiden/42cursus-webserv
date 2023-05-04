#ifndef __SERVER_HPP__
# define __SERVER_HPP__

# include <webserv.hpp>

class Server
{
	private:
		std::vector<ServerBlock> config;
	public:
		int	importConfig(const std::string filename);
		void getServer(std::ifstream& configFile);
		void listenComp(std::string line, ServerBlock& server);
		void importHost(std::string& host, ServerBlock& server);
};

#endif /* __SERVER_HPP__ */