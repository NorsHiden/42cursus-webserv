/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelidris <nelidris@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 18:44:24 by nelidris          #+#    #+#             */
/*   Updated: 2023/05/14 08:24:11 by nelidris         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

# include <webserv.hpp>

struct Client
{
	std::map<std::string, std::string> header;
	std::string	request_buffer;
	std::string	header_buffer;
	std::vector<std::string> start_line;
	ServerBlock server;
	std::pair<std::string, LocationBlock> location;
	Response	response;
	sockaddr_in	addr;
	socklen_t	addr_len;
	int			sock_fd;
	int			action;
	bool		header_done;
	char		*body;
	size_t		body_size;

	Client(): sock_fd(-1), action(false), header_done(false), body(NULL), body_size(0) {}
	~Client() { if (body) delete body; }

	void handleRequest(short port, std::vector<ServerBlock>& config);
	int setupResponse(short port, std::vector<ServerBlock>& config);
	
	// parsing request
	void readRequest(void);
	void parseRequestHeader(void);
	void parseHeaderLine(std::string& line);
	// handling errors
	ServerBlock& findHost(short port, std::vector<ServerBlock>& config);
	int setupLocation();
	int	checkMaxBodySize();
	// // error status codes
	int setupBadRequest(std::map<short, std::string>& error_pages);
	int setupForbidden(std::map<short, std::string>& error_pages);
	int setupNotFound(std::map<short, std::string>& error_pages);
	int setupHTTPVersionNotSupported(std::map<short, std::string>& error_pages);
	int setupMethodNotAllowed(std::map<short, std::string>& error_pages);
	int setupRequestEntityTooLarge(std::map<short, std::string>& error_pages);
	int setupLengthRequired(std::map<short, std::string>& error_pages);
	int setupInternalServerError(std::map<short, std::string>& error_pages);
	// finding right response
	int setupRedirection(void);
	int setupCGI(void);
	int setupUpload(void);
	int setupRegularResponse(void);
	std::string getDirectoryContent(const std::string& path);
	int aDirectoryRequest(std::string& path, std::string& filename);
	int	setupAutoIndex(std::string& path);
	// sending response
	void sendRegularResponse(void);
	void sendAutoIndexAndRedirection(void);
	
};

#endif /* __CLIENT_HPP__ */