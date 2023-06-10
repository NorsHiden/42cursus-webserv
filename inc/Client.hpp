/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelidris <nelidris@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 18:44:24 by nelidris          #+#    #+#             */
/*   Updated: 2023/06/10 21:21:26 by nelidris         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __CLIENT_HPP__
# define __CLIENT_HPP__

# include <webserv.hpp>

struct chucked_body
{
	std::vector<char>	body_buffer;
	size_t	size;
	bool	looking_for_size;
	size_t	grabbed_size;
	bool	chunked_body;
	bool	reading_done;

	chucked_body(): size(0), looking_for_size(true), grabbed_size(0), chunked_body(false), reading_done(false) {}
};

struct CGI
{
	std::string filepath;
	std::string pathinfo;
	char		**env;
	// char 		*buffer;
	std::vector<char>	body_buffer;
	// size_t		buffer_size;
	// size_t		buffer_pos;
	pid_t		pid;
	int			server_to_cgi[2];
	int			cgi_to_server[2];
	int			step;

	CGI(): env(NULL), pid(-1), step(0) {}
	~CGI() {
		if (env) {
			for (int i = 0; env[i]; i++)
				delete env[i];
			delete env;
		}
	}
};

struct Client
{
	std::pair<std::string, LocationBlock>	location;
	std::map<std::string, std::string>		header;
	std::vector<std::string>				start_line;
	chucked_body	chunked_body;
	std::string		request_buffer;
	std::string		header_buffer;
	ServerBlock		server;
	sockaddr_in		addr;
	socklen_t		addr_len;
	Response		response;
	CGI				cgi;
	std::vector<char>	body_buffer;
	int				sock_fd;
	int				action;
	bool			header_done;
	ssize_t			read_size;

	Client(): sock_fd(-1), action(false), header_done(false), read_size(0) {}
	~Client() {}

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
	int getPathInfo(void);
	void setupCGIEnv(void);
	void setupCGIstdin(void);
	int setupCGI(void);
	int setupUpload(void);
	int setupRegularResponse(void);
	std::string getDirectoryContent(const std::string& path);
	int aDirectoryRequest(std::string& path, std::string& filename);
	int	setupAutoIndex(std::string& path);
	int setupDelete(std::string filename);
	// sending response
	void sendRegularResponse(void);
	void sendAutoIndexAndRedirection(void);
	void chunkedUpload(void);
	void normalUpload(void);
	void sendUploadResponse(void);
	void sendCGIResponse(void);
	void executeCGI(void);
	void sendBodyToCGI(void);
	int waitForCGI(void);
	void readFromCGI(void);
	void CGIResponse(void);
	void sendDeleteResponse(void);
	
};

#endif /* __CLIENT_HPP__ */