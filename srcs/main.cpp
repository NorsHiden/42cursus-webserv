#include <webserv.hpp>



int main(int ac, char **av)
{
	Server server;
	if (ac != 2)
	{
		std::cerr << "Bad Arguments." << std::endl;
		return (1);
	}
	if (server.importConfig(av[1]))
		return (1);
	return (0);
}