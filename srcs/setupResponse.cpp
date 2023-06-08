/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setupResponse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelidris <nelidris@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 18:44:06 by nelidris          #+#    #+#             */
/*   Updated: 2023/06/08 15:21:09 by nelidris         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>

int Client::setupRedirection(void)
{
	response.header = "HTTP/1.1 " + stringify(location.second.redirection.second) + "\r\nServer: webserv\r\n";
	response.header += "Date: " + getCurrentTime() + "\r\n";
	response.header += "Location: " + location.second.redirection.first + "\r\n";
	response.header += "Content-Length: 0\r\n";
	response.header += "Connection: close\r\n\r\n";
	action = REDIRECTION_RESPONSE;
	return (1);
}

int Client::getPathInfo(void)
{
	size_t path = start_line[1].find('.');
	if (path == std::string::npos)
		return (1);
	path = start_line[1].find('/', path);
	if (path == std::string::npos && location.second.cgi.second.compare(&start_line[1][path + 1]))
	{
		cgi.filepath = location.second.root + start_line[1].substr(location.first.size(), start_line[1].size() - location.first.size());
		cgi.pathinfo = "/";

	}	
	else
	{
		cgi.filepath = location.second.root + start_line[1].substr(location.first.size(), path - location.first.size());
		cgi.pathinfo = start_line[1].substr(path, start_line[1].size() - path);
	}
	return (0);
}

void Client::setupCGIEnv(void)
{
	cgi.env = new char*[header.size() + 4]();
	int i = 0;
	for (std::map<std::string, std::string>::iterator it = header.begin(); it != header.end(); it++)
	{
		cgi.env[i] = new char[it->first.size() + it->second.size() + 2 + 5](); // +5 for HTTP_
		std::string env = convertToCGIHeader(it->first) + "=" + it->second;
		std::copy(env.begin(), env.end(), cgi.env[i]);
		i++;
	}
	cgi.env[i++] = strdup("SERVER_PROTOCOL=HTTP/1.1");
	cgi.env[i++] = strdup(("REQUEST_METHOD=" + start_line[0]).c_str());
	cgi.env[i++] = strdup(("PATH_INFO=" + cgi.pathinfo).c_str());
}

int Client::setupCGI(void)
{
	if (access(location.second.cgi.first.c_str(), X_OK))
		return (setupInternalServerError(server.error_pages));
	if (location.second.root.empty())
		return (setupForbidden(server.error_pages));
	if (getPathInfo())
		return (0);
	if (access(cgi.filepath.c_str(), R_OK))
		return (setupNotFound(server.error_pages));
	setupCGIEnv();
	pipe(cgi.server_to_cgi);
	pipe(cgi.cgi_to_server);
	action = CGI_RESPONSE;
	return (1);
}

int Client::setupUpload(void)
{
	if (!is_directory(location.second.upload.c_str()) && access(location.second.upload.c_str(), W_OK))
		return (setupInternalServerError(server.error_pages));
	if (header.find("Content-Length") == header.end())
		return (setupLengthRequired(server.error_pages));
	std::string path = location.second.upload + (start_line[1].substr(location.first.size(), start_line[1].size() - location.first.size()));
	if (!access(path.c_str(), F_OK))
		return (setupConflict(server.error_pages));
	response.upload_fd = open(path.c_str(), O_WRONLY | O_CREAT, 0666);
	if (response.upload_fd < 0)
		return (setupNotFound(server.error_pages));
	response.header = "HTTP/1.1 201 Created\r\nServer: webserv\r\n";
	response.header += "Date: " + getCurrentTime() + "\r\n";
	response.header += "Content-Length: 0\r\n";
	response.header += "Connection: close\r\n\r\n";
	action = UPLOAD_RESPONSE;
	return (1);
}

std::string Client::getDirectoryContent(const std::string& path)
{
	std::string content = "<html><head><title>Index of " + start_line[1] + "</title></head><body>";
	content += "<h1>Index of " + start_line[1] + "</h1><hr><pre>";
	DIR* dir = opendir(path.c_str());
	if (dir)
	{
		struct dirent* entry;
		std::string start = start_line[1];
		if (start[start.size() - 1] == '/')
			start = start.substr(0, start.size() - 1);
		while ((entry = readdir(dir)))
		{
			if (std::string(entry->d_name) != ".")
			{
				content += std::string("<div><a style=\"display:inline-block;\" href=\"")
						+ start + '/' + entry->d_name + "\">" + entry->d_name
						+ "</a><p style=\"display:inline-block;\">\t\t";
				struct stat fileStat;
				stat((path + "/" + entry->d_name).c_str(), &fileStat);
				content += stringify(fileStat.st_size) + "B "
						+ std::string(ctime(&fileStat.st_ctime)) + "</p></div>";
			}
		}
		closedir(dir);
	}
	content += "</pre><hr></body></html>";
	return (content);
}

int Client::setupAutoIndex(std::string& path)
{
	response.autoindex_body = getDirectoryContent(path);
	response.header = "HTTP/1.1 200 OK\r\nServer: webserv\r\n";
	response.header += "Date: " + getCurrentTime() + "\r\n";
	response.header += "Content-Type: text/html\r\nContent-Length: " + stringify(response.autoindex_body.size()) + "\r\n";
	response.header += "Connection: close\r\n\r\n";
	action = AUTOINDEX_RESPONSE;
	return (1);
}

int Client::aDirectoryRequest(std::string& path, std::string& filename)
{
	path += '/';
	for (size_t i = 0; i < location.second.index.size(); ++i)
	{
		if (!access((path + location.second.index[i]).c_str(), R_OK))
		{
			filename = path + location.second.index[i];
			break ;
		}
	}
	if (filename.empty())
	{
		if (!location.second.autoindex)
			return (setupForbidden(server.error_pages));
		return (setupAutoIndex(path));
	}
	return (0);
}

int Client::setupRegularResponse(void)
{
	if (!is_directory(location.second.root.c_str()) || access(location.second.root.c_str(), R_OK))
		return (setupForbidden(server.error_pages));
	std::string path = location.second.root + (start_line[1].substr(location.first.size(), start_line[1].size() - location.first.size()));
	std::string filename;
	if (is_directory(path.c_str()))
	{
		if (aDirectoryRequest(path, filename))
			return (1);
	}
	else if (!access(path.c_str(), R_OK))
		filename = path;
	else
		return ((setupNotFound(server.error_pages)));
	response.body_fd = open(filename.c_str(), O_RDONLY);
	response.header = "HTTP/1.1 200 OK\r\nServer: webserv\r\n";
	response.header += "Date: " + getCurrentTime() + "\r\n";
	response.header += "Content-Type: " + getFileType(filename) + "\r\nContent-Length: " + stringify(getFileSize(filename)) + "\r\n";
	response.header += "Connection: close\r\n\r\n";
	action = REGULAR_RESPONSE;
	return (1);
}


ServerBlock& Client::findHost(short port, std::vector<ServerBlock>& config)
{
	ServerBlock* matchedServer = NULL;
	for (size_t i = 0; i < config.size(); ++i)
	{
		if (config[i].listen.second == port)
		{
			std::vector<std::string> hostname = split(header["Host"], ':');
			for (size_t j = 0; j < config[i].server_names.size(); j++)
			{
				if (hostname[0] == config[i].server_names[j])
					return (config[i]);
			}
			if (!matchedServer)
				matchedServer = &config[i];
		}
	}
	if (matchedServer)
		return (*matchedServer);
	return (config[0]);
}

int Client::setupLocation(void)
{
	std::map<std::string, LocationBlock> matchedLocations = server.locations;
	std::string path;
	std::map<std::string, LocationBlock>::iterator it = matchedLocations.begin();
	while (it != matchedLocations.end())
	{
		if (start_line[1].compare(0, it->first.size(), it->first))
			matchedLocations.erase(it++);
		else if (path.size() < it->first.size())
			path = (it++)->first;
	}
	if (path.empty())
		return (setupNotFound(server.error_pages));
	location = *matchedLocations.find(path);
	return (0);
}

int Client::checkMaxBodySize(void)
{
	ssize_t content_size = 0;
	if (start_line[0] != "POST")
		return (0);
	if (header.find("Content-Length") == header.end() && header.find("Transfer-Encoding") == header.end())
		return (setupLengthRequired(server.error_pages));
	try
	{
		content_size = stringToLong(header["Content-Length"]);
		if (server.client_max_body_size < content_size)
			return (setupRequestEntityTooLarge(server.error_pages));
	}
	catch (std::invalid_argument& e)
	{
		return (setupBadRequest(server.error_pages));
	}
	return (0);
}


int Client::setupResponse(short port, std::vector<ServerBlock>& config)
{
	// error handling
	if (start_line.size() != 3)
		return (setupBadRequest(config[0].error_pages));
	if (start_line[2] != "HTTP/1.1\r")
		return (setupHTTPVersionNotSupported(config[0].error_pages));
	server = findHost(port, config);
	if (checkMaxBodySize())
		return (1);
	if (setupLocation())
		return (setupNotFound(server.error_pages));
	if (location.second.allowed_methods.find(start_line[0]) == location.second.allowed_methods.end())
		return (setupMethodNotAllowed(server.error_pages));
	// finding right response
	if (!location.second.redirection.first.empty())
		return (setupRedirection());
	if (!location.second.cgi.first.empty())
	{
		if (setupCGI())
			return (1);
	}
	if (start_line[0] == "POST" && !location.second.upload.empty())
		return (setupUpload());
	setupRegularResponse();
	return (0);
}
