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
		std::cout << server;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}