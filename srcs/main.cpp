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
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	std::cout << server;
	return (0);
}