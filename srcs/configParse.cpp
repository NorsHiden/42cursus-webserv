#include <webserv.hpp>

void Server::importHost(std::string& host, ServerBlock& server)
{
	size_t ip_addr = 0;

	for (size_t i = 0; isspace(host[i]); host = host.substr(1, host.size() - 1))
		;
	std::vector<std::string> splitted_ip = split(host, '.');
	if (splitted_ip.size() != 4)
		throw (std::runtime_error("invalid listen value."));
	for (int times = 0; times < 4; times++)
	{
		size_t part = stringToLong(splitted_ip[times]);
		if (part < 0 || part > 255)
			throw (std::runtime_error("invalid listen value."));
		ip_addr += part;
		ip_addr <<= 8;
	}
	if (server.listen.first >= 0)
			throw (std::runtime_error("duplication in listen value."));
	server.listen.first = ip_addr;
}

void Server::listenComp(std::string line, ServerBlock& server)
{
	std::string value = line.substr(8, line.size() - 8);

	if (emptyLine(value))
		throw (std::runtime_error("listen has no value."));
	std::vector<std::string> host_port = split(value, ':');
	if (host_port.size() != 2)
		throw (std::runtime_error("invalid listen value."));
	importHost(host_port[0], server);
	
	// import port
	size_t port = stringToLong(host_port[1]);
	server.listen.second = port;
}

void Server::getServer(std::ifstream& configFile)
{
	ServerBlock server;
	for (std::string line; std::getline(configFile, line);)
	{
		if (emptyLine(line))
			continue ;
		if (!line.compare(0, 8, "\tlisten:"))
			listenComp(line, server);
		else if (!line.compare(0, 14, "\tserver_names:"))
			serverNamesComp(line, server);
		else if (!line.compare(0, 22, "\tclient_max_body_size:"))
			clientMaxBodySizeComp(line, server);
		else if (!line.compare(0, 13, "\terror_pages:"))
			errorPagesComp(line, server);
		else if (!line.compare(0, 10, "\tlocation:"))
			locationComp(line, configFile, server);
		else if (!line.compare("end"))
			break ;
	}
}

int	Server::importConfig(const std::string filename)
{
	std::ifstream configFile(filename.data());

	if (!configFile.is_open())
	{
		std::cerr << "Unable to read from " << filename << std::endl;
		return (1);
	}
	for (std::string line; std::getline(configFile, line);)
	{
		if (!line.compare(0, 7, "server:"))
			getServer(configFile);
		
	}
	return (0);
}