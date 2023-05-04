#ifndef _WEBSERV_HPP_
# define _WEBSERV_HPP_

# include <iostream>
# include <vector>
# include <map>
# include <utility>
# include <fstream>
# include <sstream>

# include "LocationBlock.hpp"
# include "ServerBlock.hpp"
# include "Server.hpp"

bool emptyLine(const std::string& line);
std::vector<std::string> split(const std::string& str, char delim);
long int stringToLong(const std::string& str);

#endif /* _WEBSERV_HPP_ */