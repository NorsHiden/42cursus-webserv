#ifndef __SERVERBLOCK_HPP__
# define __SERVERBLOCK_HPP__

# include <webserv.hpp>

struct ServerBlock
{
	std::pair<size_t, short> listen; // [ IP_ADDR, PORT ]
	std::vector<std::string> server_names;
	size_t client_max_body_size; // IN BYTES ONLY
	std::map<short, std::string> error_pages; // [ PATH, STATUS_CODE ]
	std::map<std::string, LocationBlock> locations; // [ URI, LOCATION_BLOCK ]

	ServerBlock(): client_max_body_size(0) {}
};

#endif /* __SERVERBLOCK_HPP__ */