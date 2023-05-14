/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelidris <nelidris@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 18:44:43 by nelidris          #+#    #+#             */
/*   Updated: 2023/05/13 18:44:43 by nelidris         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _WEBSERV_HPP_
# define _WEBSERV_HPP_

# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/select.h>
# include <unistd.h>
# include <iostream>
# include <dirent.h>
# include <string.h>
# include <sys/socket.h>
# include <fcntl.h>
# include <ctime>
# include <sys/stat.h>
# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <vector>
# include <set>
# include <map>
# include <list>
# include <utility>
# include <fstream>
# include <sstream>
# include <algorithm>

# include "LocationBlock.hpp"
# include "ServerBlock.hpp"
# include "Response.hpp"
# include "Client.hpp"
# include "Server.hpp"

# define LISTEN_QUEUE 5
# define BUFFER_DATA 1024

# define READ_SOCKET 0
# define SETUP_RESPONSE 1
# define REGULAR_RESPONSE 2
# define AUTOINDEX_RESPONSE 3
# define UPLOAD_RESPONSE 4
# define CGI_RESPONSE 5
# define REDIRECTION_RESPONSE 6
# define ERROR_RESPONSE 7
# define REMOVE_CLIENT 8

bool emptyLine(const std::string& line);
std::vector<std::string> split(const std::string& str, char delim);
long int stringToLong(const std::string& str);
void appendToBody(char **body, size_t &body_size, char *buffer, size_t buffer_size);
std::string removeWhiteSpace(std::string line);
std::string getCurrentTime();
std::string stringify(long value);
long getFileSize(const std::string& filename);
bool is_directory(const char* path);
std::string getFileType(std::string filename);

#endif /* _WEBSERV_HPP_ */