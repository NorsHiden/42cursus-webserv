/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configParse.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelidris <nelidris@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 18:43:27 by nelidris          #+#    #+#             */
/*   Updated: 2023/05/17 08:22:37 by nelidris         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>

void Server::importHost(std::string& host, ServerBlock& server)
{
	size_t ip_addr = 0;

	std::vector<std::string> splitted_ip = split(host, '.');
	if (splitted_ip.size() != 4)
		throw (std::runtime_error("invalid listen value."));
	for (int times = 0; times < 4; times++)
	{
		int part = stringToLong(splitted_ip[times]);
		if (part < 0 || part > 255)
			throw (std::runtime_error("invalid listen value."));
		ip_addr <<= 8;
		ip_addr += part;
	}
	if (server.listen.first > 0)
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

void Server::serverNamesComp(std::string line, ServerBlock& server)
{
	std::string value = line.substr(14, line.size() - 14);
	if (emptyLine(value))
		throw (std::runtime_error("server_names has no value."));
	if (!server.server_names.empty())
			throw (std::runtime_error("duplication in server_names value."));
	std::vector<std::string> domains = split(value, ' ');
	server.server_names = domains;
}

void Server::clientMaxBodySizeComp(std::string line, ServerBlock& server)
{
	std::string value = line.substr(22, line.size() - 22);
	if (emptyLine(value))
		throw (std::runtime_error("client_max_body_size has no value."));
	
	if (server.client_max_body_size)
			throw (std::runtime_error("duplication in client_max_body_size value."));
	std::vector<std::string> bytes = split(value, ' ');
	if (bytes.size() != 1 || bytes[0][0] == '-')
		throw (std::runtime_error("invalid client_max_body_size value."));
	server.client_max_body_size = stringToLong(bytes[0]);
}

void Server::errorPagesComp(std::string line, ServerBlock& server)
{
	std::string value = line.substr(13, line.size() - 13);
	if (emptyLine(value))
		throw (std::runtime_error("error_page has no value."));
	
	std::vector<std::string> status_path = split(value, ':');
	if (status_path.size() != 2)
		throw (std::runtime_error("invalid error_page value."));
	std::pair<short, std::string> page;
	page.first = stringToLong(status_path[0]);
	if (page.first < 300 || page.first > 599)
		throw (std::runtime_error("error_page value should be between 300-599."));
	else if (server.error_pages.find(page.first) != server.error_pages.end())
		throw (std::runtime_error("duplication in error_page value."));
	page.second = status_path[1];
	server.error_pages.insert(page);
}

void Server::rootComp(std::string line, LocationBlock& location)
{
	if (!location.root.empty())
		throw (std::runtime_error("duplication in root value."));
	std::string value = line.substr(7, line.size() - 7);
	if (emptyLine(value))
		throw (std::runtime_error("root has no value."));
	std::vector<std::string> path = split(value, ' ');
	if (path.size() != 1)
		throw (std::runtime_error("invalid root value."));
	if (path[0][path[0].size() - 1] != '/')
		path[0] += '/';
	location.root = path[0];
}

void Server::allowedMethodsComp(std::string line, LocationBlock& location)
{
	if (!location.allowed_methods.empty())
		throw (std::runtime_error("duplication in allowed_methods value."));
	std::string value = line.substr(18, line.size() - 18);
	if (emptyLine(value))
		throw (std::runtime_error("allowed_methods has no value."));
	std::vector<std::string> methods = split(value, ' ');
	for (size_t i = 0; i < methods.size(); i++)
	{
		if (methods[i] != "GET" && methods[i] != "POST" && methods[i] != "DELETE")
			throw (std::runtime_error("invalid allowed_methods value."));
		location.allowed_methods.insert(methods[i]);
	}
}

void Server::indexComp(std::string line, LocationBlock& location)
{
	if (!location.index.empty())
		throw (std::runtime_error("duplication in index value."));
	std::string value = line.substr(8, line.size() - 8);
	if (emptyLine(value))
		throw (std::runtime_error("index has no value."));
	std::vector<std::string> indexes = split(value, ' ');
	location.index = indexes;
}

void Server::cgiComp(std::string line, LocationBlock& location)
{
	if (!location.cgi.first.empty())
		throw (std::runtime_error("duplication in cgi value."));
	std::string value = line.substr(6, line.size() - 6);
	if (emptyLine(value))
		throw (std::runtime_error("cgi has no value."));
	std::vector<std::string> path_ext = split(value, ' ');
	if (path_ext.size() != 2)
		throw (std::runtime_error("invalid cgi value."));
	location.cgi.first = path_ext[0];
	location.cgi.second = path_ext[1];
}

void Server::autoIndexComp(std::string line, LocationBlock& location)
{
	std::string value = line.substr(12, line.size() - 12);
	if (emptyLine(value))
		throw (std::runtime_error("autoindex has no value."));
	std::vector<std::string> bin = split(value, ' ');
	if (bin.size() != 1)
		throw (std::runtime_error("invalid autoindex value."));
	if (bin[0] == "true" || bin[0] == "on")
		location.autoindex = true;
	else if (bin[0] == "false" || bin[0] == "off")
		location.autoindex = false;
	else
		throw (std::runtime_error("invalid autoindex value."));
}

void Server::uploadComp(std::string line, LocationBlock& location)
{
	std::string value = line.substr(9, line.size() - 9);
	if (emptyLine(value))
		throw (std::runtime_error("upload has no path."));
	std::vector<std::string> path = split(value, ' ');
	if (path.size() != 1)
		throw (std::runtime_error("invalid upload path."));
	if (!location.upload.empty())
		throw (std::runtime_error("duplication in upload path."));
	location.upload = path[0];
}

void Server::redirectionComp(std::string line, LocationBlock& location)
{
	std::string value = line.substr(9, line.size() - 9);
	if (emptyLine(value))
		throw (std::runtime_error("return has no value."));
	std::vector<std::string> redir = split(value, ' ');
	if (redir.size() != 2)
		throw (std::runtime_error("invalid return value."));
	location.redirection.first = redir[0];
	short status = stringToLong(redir[1]);
	if (status < 0 || status > 999)
		throw (std::runtime_error("invalid return value."));
	location.redirection.second = status;
}

void Server::locationComp(std::string line, std::ifstream& configFile, ServerBlock& server)
{
	std::string value = line.substr(10, line.size() - 10);
	if (emptyLine(value))
		throw (std::runtime_error("location has no uri."));
	std::vector<std::string> uri = split(value, ' ');
	if (uri.size() != 1)
		throw (std::runtime_error("invalid location uri."));
	std::pair<std::string, LocationBlock> location;
	location.first = uri[0];

	for (std::string line; std::getline(configFile, line);)
	{
		if (emptyLine(line))
			continue ;
		if (!line.compare(0, 7, "\t\troot:"))
			rootComp(line, location.second);
		else if (!line.compare(0, 18, "\t\tallowed_methods:"))
			allowedMethodsComp(line, location.second);
		else if (!line.compare(0, 8, "\t\tindex:"))
			indexComp(line, location.second);
		else if (!line.compare(0, 6, "\t\tcgi:"))
			cgiComp(line, location.second);
		else if (!line.compare(0, 12, "\t\tautoindex:"))
			autoIndexComp(line, location.second);
		else if (!line.compare(0, 9, "\t\tupload:"))
			uploadComp(line, location.second);
		else if (!line.compare(0, 9, "\t\treturn:"))
			redirectionComp(line, location.second);
		else if (!line.compare(0, 4, "\tend"))
		{
			server.locations.insert(location);
			break ;
		}
		else
			throw (std::runtime_error("invalid location directive."));
	}
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
		else if (!line.compare(0, 12, "\terror_page:"))
			errorPagesComp(line, server);
		else if (!line.compare(0, 10, "\tlocation:"))
			locationComp(line, configFile, server);
		else if (line == "end")
		{
			config.push_back(server);
			break ;
		}
		else
			throw (std::runtime_error("invalid server directive."));
	}
}

void Server::importConfig(const std::string& filename)
{
	std::ifstream configFile(filename.data());

	if (!configFile.is_open())
		throw (std::runtime_error("unable to import the configuration file."));
	for (std::string line; std::getline(configFile, line);)
	{
		if (emptyLine(line))
			continue ;
		if (!line.compare(0, 7, "server:"))
			getServer(configFile);
		else
			throw (std::runtime_error("invalid directive."));
	}
}

std::ostream& operator<<(std::ostream& out, Server& server)
{
	for (size_t i = 0; i < server.config.size(); i++)
	{
		std::cout << "server " << i << ":" << std::endl;
		std::cout << "\tlisten: " << server.config[i].listen.first << ":" << server.config[i].listen.second << std::endl;
		std::cout << "\terror_pages:" << std::endl;
		for (std::map<short, std::string>::iterator it = server.config[i].error_pages.begin(); it != server.config[i].error_pages.end(); it++)
			std::cout << "\t\t" << it->first << ":" << it->second << std::endl;
		std::cout << "\tclient_max_body_size: " << server.config[i].client_max_body_size << std::endl;
		std::cout << "\tserver_names: ";
		for (size_t j = 0; j < server.config[i].server_names.size(); j++)
			std::cout << server.config[i].server_names[j] << " ";
		std::cout << std::endl;
		for (std::map<std::string, LocationBlock>::iterator it = server.config[i].locations.begin(); it != server.config[i].locations.end(); it++)
		{
			std::cout << "\tlocation: " << it->first << std::endl;
			std::cout << "\t\troot: " << it->second.root << std::endl;
			std::cout << "\t\tupload: " << it->second.upload << std::endl;
			std::cout << "\t\tallowed_methods: ";
			for (std::set<std::string>::iterator ite = it->second.allowed_methods.begin(); ite != it->second.allowed_methods.end(); ite++)
				std::cout << *ite << " ";
			std::cout << std::endl;
			std::cout << "\t\tautoindex: " << it->second.autoindex << std::endl;
			std::cout << "\t\tcgi: " << it->second.cgi.first << ' ' << it->second.cgi.second << std::endl;
			std::cout << "\t\tredirection: " << it->second.redirection.first << ' ' << it->second.redirection.second << std::endl;
			std::cout << "\t\tindex: ";
			for (std::vector<std::string>::iterator ite = it->second.index.begin(); ite != it->second.index.end(); ite++)
				std::cout << *ite << " ";
			std::cout << std::endl;
		}
	}
	return (out);
}