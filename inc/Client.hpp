/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelidris <nelidris@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 18:44:24 by nelidris          #+#    #+#             */
/*   Updated: 2023/05/15 07:36:35 by nelidris         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

# include <webserv.hpp>

struct chucked_body
{
	size_t	size;
	char	*buffer;
	bool	looking_for_size;
	size_t	grabbed_size;
	bool	chunked_body;
	bool	reading_body;
	size_t	pos_body;
	size_t	start_body;
	bool	reading_done;

	chucked_body(): size(0), buffer(NULL), looking_for_size(true), grabbed_size(0), chunked_body(false),
		reading_body(false), pos_body(0), start_body(0), reading_done(false) {}
	~chucked_body() { if (buffer) delete buffer; }
};

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
	chucked_body	chunked_body;
	bool		header_done;
	size_t		pos;
	char		*body;
	size_t		body_size;

	Client(): sock_fd(-1), action(false), header_done(false), pos(0), body(NULL), body_size(0) {}
	~Client() { if (body) delete body; }

	void handleRequest(short port, std::vector<ServerBlock>& config);
	int setupResponse(short port, std::vector<ServerBlock>& config);
	
	// parsing request
	void readRequest(void);
	void adjustBody(void);
	void parseRequestHeader(void);
	void parseHeaderLine(std::string& line);
	// handling errors
	ServerBlock& findHost(short port, std::vector<ServerBlock>& config);
	int setupLocation();
	int	checkMaxBodySize();
	// // error status codes
	int setupBadRequest(std::map<short, std::string>& error_pages);
	int setupForbidden(std::map<short, std::string>& error_pages);
	int setupConflict(std::map<short, std::string>& error_pages);
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
	void chunkedUpload(void);
	void normalUpload(void);
	void sendUploadResponse(void);
	
};

#endif /* __CLIENT_HPP__ */