#ifndef __LOCATIONBLOCK_HPP__
# define __LOCATIONBLOCK_HPP__

# include <webserv.hpp>

struct LocationBlock
{
	std::string root;
	std::set<std::string> allowed_methods; // [ GET, POST, DELETE ] ONLY
	std::vector<std::string> index;
	std::pair<std::string, std::string> cgi; // [ PATH, EXTENSION ]
	bool autoindex;
	std::string upload; // PATH
	std::pair<std::string, short> redirection; // [ URI, STATUS_CODE ]
};

#endif /* __LOCATIONBLOCK_HPP__ */