/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errorRequest.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelidris <nelidris@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 18:43:32 by nelidris          #+#    #+#             */
/*   Updated: 2023/05/14 15:15:10 by nelidris         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>

int Client::setupBadRequest(std::map<short, std::string>& error_pages)
{
	response.header = "HTTP/1.1 400 Bad Request\r\nServer: webserv\r\n";
	std::string path;
	if (error_pages.find(400) != error_pages.end())
	{
		if (access(error_pages[400].c_str(), R_OK) == -1)
			path = "default_pages/400.html";
		else
			path = error_pages[400];
	}
	else
		path = "default_pages/400.html";
	response.body_fd = open(path.c_str(), O_RDONLY);
	response.header += "Date: " + getCurrentTime() + "\r\n";
	response.header += "Content-Type: text/html\r\nContent-Length: " + stringify(getFileSize(path)) + "\r\n";
	response.header += "Connection: keep-alive\r\n\r\n";
	action = ERROR_RESPONSE;
	return (1);
}

int Client::setupConflict(std::map<short, std::string>& error_pages)
{
	response.header = "HTTP/1.1 409 Conflict\r\nServer: webserv\r\n";
	std::string path;
	if (error_pages.find(409) != error_pages.end())
	{
		if (access(error_pages[409].c_str(), R_OK) == -1)
			path = "default_pages/409.html";
		else
			path = error_pages[409];
	}
	else
		path = "default_pages/409.html";
	response.body_fd = open(path.c_str(), O_RDONLY);
	response.header += "Date: " + getCurrentTime() + "\r\n";
	response.header += "Content-Type: text/html\r\nContent-Length: " + stringify(getFileSize(path)) + "\r\n";
	response.header += "Connection: keep-alive\r\n\r\n";
	action = ERROR_RESPONSE;
	return (1);
}

int Client::setupLengthRequired(std::map<short, std::string>& error_pages)
{
	response.header = "HTTP/1.1 411 Length Required\r\nServer: webserv\r\n";
	std::string path;
	if (error_pages.find(411) != error_pages.end())
	{
		if (access(error_pages[411].c_str(), R_OK) == -1)
			path = "default_pages/411.html";
		else
			path = error_pages[411];
	}
	else
		path = "default_pages/411.html";
	response.body_fd = open(path.c_str(), O_RDONLY);
	response.header += "Date: " + getCurrentTime() + "\r\n";
	response.header += "Content-Type: text/html\r\nContent-Length: " + stringify(getFileSize(path)) + "\r\n";
	response.header += "Connection: keep-alive\r\n\r\n";
	action = ERROR_RESPONSE;
	return (1);
}

int Client::setupInternalServerError(std::map<short, std::string>& error_pages)
{
	response.header = "HTTP/1.1 500 Internal Server Error\r\nServer: webserv\r\n";
	std::string path;
	if (error_pages.find(500) != error_pages.end())
	{
		if (access(error_pages[500].c_str(), R_OK) == -1)
			path = "default_pages/500.html";
		else
			path = error_pages[500];
	}
	else
		path = "default_pages/500.html";
	response.body_fd = open(path.c_str(), O_RDONLY);
	response.header += "Date: " + getCurrentTime() + "\r\n";
	response.header += "Content-Type: text/html\r\nContent-Length: " + stringify(getFileSize(path)) + "\r\n";
	response.header += "Connection: keep-alive\r\n\r\n";
	action = ERROR_RESPONSE;
	return (1);
}

int Client::setupRequestEntityTooLarge(std::map<short, std::string>& error_pages)
{
	response.header = "HTTP/1.1 413 Request Entity Too Large\r\nServer: webserv\r\n";
	std::string path;
	if (error_pages.find(413) != error_pages.end())
	{
		if (access(error_pages[413].c_str(), R_OK) == -1)
			path = "default_pages/413.html";
		else
			path = error_pages[413];
	}
	else
		path = "default_pages/413.html";
	response.body_fd = open(path.c_str(), O_RDONLY);
	response.header += "Date: " + getCurrentTime() + "\r\n";
	response.header += "Content-Type: text/html\r\nContent-Length: " + stringify(getFileSize(path)) + "\r\n";
	response.header += "Connection: keep-alive\r\n\r\n";
	action = ERROR_RESPONSE;
	return (1);
}

int Client::setupMethodNotAllowed(std::map<short, std::string>& error_pages)
{
	response.header = "HTTP/1.1 405 Method Not Allowed\r\nServer: webserv\r\n";
	std::string path;
	if (error_pages.find(405) != error_pages.end())
	{
		if (access(error_pages[405].c_str(), R_OK) == -1)
			path = "default_pages/405.html";
		else
			path = error_pages[405];
	}
	else
		path = "default_pages/405.html";
	response.body_fd = open(path.c_str(), O_RDONLY);
	response.header += "Date: " + getCurrentTime() + "\r\n";
	response.header += "Content-Type: text/html\r\nContent-Length: " + stringify(getFileSize(path)) + "\r\n";
	response.header += "Connection: keep-alive\r\n\r\n";
	action = ERROR_RESPONSE;
	return (1);
}

int Client::setupHTTPVersionNotSupported(std::map<short, std::string>& error_pages)
{
	response.header = "HTTP/1.1 505 HTTP Version Not Supported\r\nServer: webserv\r\n";
	std::string path;
	if (error_pages.find(505) != error_pages.end())
	{
		if (access(error_pages[505].c_str(), R_OK) == -1)
			path = "default_pages/505.html";
		else
			path = error_pages[505];
	}
	else
		path = "default_pages/505.html";
	response.body_fd = open(path.c_str(), O_RDONLY);
	response.header += "Date: " + getCurrentTime() + "\r\n";
	response.header += "Content-Type: text/html\r\nContent-Length: " + stringify(getFileSize(path)) + "\r\n";
	response.header += "Connection: keep-alive\r\n\r\n";
	action = ERROR_RESPONSE;
	return (1);
}

int Client::setupForbidden(std::map<short, std::string>& error_pages)
{
	response.header = "HTTP/1.1 403 Forbidden\r\nServer: webserv\r\n";
	std::string path;
	if (error_pages.find(403) != error_pages.end())
	{
		if (access(error_pages[403].c_str(), R_OK) == -1)
			path = "default_pages/403.html";
		else
			path = error_pages[403];
	}
	else
		path = "default_pages/403.html";
	response.body_fd = open(path.c_str(), O_RDONLY);
	response.header += "Date: " + getCurrentTime() + "\r\n";
	response.header += "Content-Type: text/html\r\nContent-Length: " + stringify(getFileSize(path)) + "\r\n";
	response.header += "Connection: keep-alive\r\n\r\n";
	action = ERROR_RESPONSE;
	return (1);
}

int Client::setupNotFound(std::map<short, std::string>& error_pages)
{
	response.header = "HTTP/1.1 404 Not Found\r\nServer: webserv\r\n";
	std::string path;
	if (error_pages.find(404) != error_pages.end())
	{
		if (access(error_pages[404].c_str(), R_OK) == -1)
			path = "default_pages/404.html";
		else
			path = error_pages[404];
	}
	else
		path = "default_pages/404.html";
	response.body_fd = open(path.c_str(), O_RDONLY);
	response.header += "Date: " + getCurrentTime() + "\r\n";
	response.header += "Content-Type: text/html\r\nContent-Length: " + stringify(getFileSize(path)) + "\r\n";
	response.header += "Connection: keep-alive\r\n\r\n";
	action = ERROR_RESPONSE;
	return (1);
}
