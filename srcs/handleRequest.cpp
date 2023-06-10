/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelidris <nelidris@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 18:43:40 by nelidris          #+#    #+#             */
/*   Updated: 2023/06/10 21:35:20 by nelidris         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>

void Client::parseHeaderLine(std::string& line)
{
	std::pair<std::string, std::string> var;
	size_t pos = line.find(':');

	if (pos != std::string::npos)
	{
		var.first = line.substr(0, pos);
		var.second = removeWhiteSpace(line.substr(pos + 1, line.size() - pos));
	}
	else
		var.first = removeWhiteSpace(line);
	header.insert(var);
}

void Client::adjustBody(void)
{
	// chunked body
	size_t pos = 0;
	while (body_buffer.size() > 0 && body_buffer.size() > pos)
	{
		if (body_buffer[pos] == '\r' && body_buffer[pos + 1] == '\n')
		{
			if (chunked_body.looking_for_size)
			{
				chunked_body.size = strtol(&body_buffer[0], NULL, 16);
				if (chunked_body.size == 0)
				{
					chunked_body.reading_done = true;
					break ;
				}
				chunked_body.looking_for_size = false;
				chunked_body.grabbed_size = 0;
				body_buffer.erase(body_buffer.begin(), body_buffer.begin() + pos + 2);
				pos = 0;
			}
			else
			{
				chunked_body.looking_for_size = true;
				chunked_body.body_buffer.insert(chunked_body.body_buffer.end(), body_buffer.begin(), body_buffer.begin() + pos);
				body_buffer.erase(body_buffer.begin(), body_buffer.begin() + pos + 2);
				chunked_body.grabbed_size += pos;
				if (chunked_body.grabbed_size == chunked_body.body_buffer.size())
					break ;
				pos = 0;
			}
		}
		pos++;
	}
}

void Client::parseRequestHeader(void)
{
	std::stringstream in(header_buffer);
	std::string line;

	std::getline(in, line);
	start_line = split(line, ' ');
	while (std::getline(in, line))
		parseHeaderLine(line);
	if (header.find("Transfer-Encoding") != header.end() && header["Transfer-Encoding"] == "chunked")
		chunked_body.chunked_body = true;
}


void Client::readRequest(void)
{
	char buffer[BUFFER_DATA + 1];
	memset(buffer, 0, sizeof(buffer));
	int ret = recv(sock_fd, buffer, BUFFER_DATA, 0);
	if (ret <= 0)
	{	
		std::cout << "connection closed" << std::endl;
		action = REMOVE_CLIENT;
		return ;
	}	// connection closed
	if (action == READ_SOCKET)
	{
		request_buffer += buffer;
		size_t pos = request_buffer.find("\r\n\r\n");
		if (pos != std::string::npos)
		{
			header_buffer = request_buffer.substr(0, pos + 4);
			request_buffer.erase(0, pos + 4);
			parseRequestHeader();
			if (!request_buffer.empty())
			{
				body_buffer.insert(body_buffer.end(), buffer + pos + 4, buffer + ret);
				if (chunked_body.chunked_body)
					adjustBody();
			}
			action = SETUP_RESPONSE;
		}
	}
	else
	{
		body_buffer.insert(body_buffer.end(), buffer, buffer + ret);
		if (chunked_body.chunked_body)
			adjustBody();
	}
}


void Client::sendRegularResponse(void)
{
	if (!response.second_time)
	{
		if (send(sock_fd, response.header.c_str(), response.header.size(), 0) < 0)
			action = REMOVE_CLIENT;
		else
			response.second_time = true;
	}
	else
	{
		char buffer[BUFFER_DATA];
		ssize_t n = read(response.body_fd, buffer, BUFFER_DATA);
		if (!n)
		{
			action = REMOVE_CLIENT;
			return ;
		}
		int sent_bytes = send(sock_fd, buffer, BUFFER_DATA, 0);
		if (sent_bytes < BUFFER_DATA)
			lseek(response.body_fd, -(BUFFER_DATA - sent_bytes), SEEK_CUR);
		return ;
	}
}

void Client::sendAutoIndexAndRedirection(void)
{
	if (action == REDIRECTION_RESPONSE)
		send(sock_fd, response.header.c_str(), response.header.size(), 0);
	else
	{
		std::vector<char> send_buffer;
		send_buffer.insert(send_buffer.end(), response.header.begin(), response.header.end());
		send_buffer.insert(send_buffer.end(), response.autoindex_body.begin(), response.autoindex_body.end());
		send(sock_fd, send_buffer.data(), send_buffer.size(), 0);
	}
	action = REMOVE_CLIENT;
}

void Client::chunkedUpload(void)
{
	size_t write_size = 0;
	if ((ssize_t)chunked_body.body_buffer.size() > BUFFER_DATA)
	{
		if (read_size + BUFFER_DATA > server.client_max_body_size)
		{
			write_size = server.client_max_body_size - read_size;
			response.second_time = true;
		}
		else
			write_size = BUFFER_DATA;
		write(response.upload_fd, &chunked_body.body_buffer[0], write_size);
		chunked_body.body_buffer.erase(chunked_body.body_buffer.begin(), chunked_body.body_buffer.begin() + write_size);
	}
	else
	{
		if (read_size + (ssize_t)chunked_body.body_buffer.size() > server.client_max_body_size)
			write_size = server.client_max_body_size - read_size;
		else
			write_size = (ssize_t)chunked_body.body_buffer.size();
		write(response.upload_fd, &chunked_body.body_buffer[0], write_size);
		if (chunked_body.reading_done)
			response.second_time = true;
		chunked_body.body_buffer.clear();
	}
	read_size += write_size;
}

void Client::normalUpload(void)
{
	size_t write_size = 0;
	if ((ssize_t)body_buffer.size() > BUFFER_DATA)
	{
		if (read_size + BUFFER_DATA > stringToLong(header["Content-Length"]))
		{
			write_size = stringToLong(header["Content-Length"]) - read_size;
			response.second_time = true;
		}
		else
			write_size = BUFFER_DATA;
		write(response.upload_fd, &body_buffer[0], write_size);
		body_buffer.erase(body_buffer.begin(), body_buffer.begin() + write_size);
	}
	else
	{
		if (read_size + (ssize_t)body_buffer.size() > stringToLong(header["Content-Length"]))
		{
			write_size = stringToLong(header["Content-Length"]) - read_size;
		}
		else
			write_size = (ssize_t)body_buffer.size();
		write(response.upload_fd, &body_buffer[0], write_size);
		if (read_size + (ssize_t)write_size == stringToLong(header["Content-Length"]))
			response.second_time = true;
		body_buffer.clear();
	}
	read_size += write_size;
}

void Client::sendUploadResponse(void)
{
	if (response.second_time)
	{
		send(sock_fd, response.header.c_str(), response.header.size(), 0);
		action = REMOVE_CLIENT;
	}
	else if (chunked_body.chunked_body)
		chunkedUpload();
	else
		normalUpload();
}

void Client::executeCGI(void)
{
	cgi.pid = fork();
	if (!cgi.pid)
	{
		dup2(cgi.server_to_cgi[0], STDIN_FILENO);
		dup2(cgi.cgi_to_server[1], STDOUT_FILENO);
		close(cgi.cgi_to_server[1]);
		close(cgi.cgi_to_server[0]);
		close(cgi.server_to_cgi[0]);
		close(cgi.server_to_cgi[1]);
		char * const argv[3] = {
			const_cast<char * const>(location.second.cgi.first.c_str()),
			const_cast<char * const>(cgi.filepath.data()), NULL
		};
		execve(location.second.cgi.first.c_str(), argv, cgi.env);
		exit(127);
	}
	close(cgi.cgi_to_server[1]);
	close(cgi.server_to_cgi[0]);
	cgi.step++;
}

void Client::sendBodyToCGI(void)
{
	size_t write_size = 0;
	if (body_buffer.empty() && chunked_body.body_buffer.empty())
	{
		close(cgi.server_to_cgi[1]);
		cgi.step++;
		return ;
	}
	if (chunked_body.chunked_body)
	{
		if ((ssize_t)chunked_body.body_buffer.size() > BUFFER_DATA)
		{
			if (read_size + BUFFER_DATA > server.client_max_body_size)
				write_size = server.client_max_body_size - read_size;
			else
				write_size = BUFFER_DATA;
			write(cgi.server_to_cgi[1], &chunked_body.body_buffer[0], write_size);
			chunked_body.body_buffer.erase(chunked_body.body_buffer.begin(), chunked_body.body_buffer.begin() + write_size);
		}
		else
		{
			if (read_size + (ssize_t)chunked_body.body_buffer.size() > server.client_max_body_size)
				write_size = server.client_max_body_size - read_size;
			else
				write_size = chunked_body.body_buffer.size();
			write(cgi.server_to_cgi[1], &chunked_body.body_buffer[0], write_size);
			chunked_body.body_buffer.clear();
		}
	}
	else
	{
		if (body_buffer.size() > BUFFER_DATA)
		{
			if (read_size + BUFFER_DATA > stringToLong(header["Content-Length"]))
				write_size = stringToLong(header["Content-Length"]) - read_size;
			else
				write_size = BUFFER_DATA;
			write(cgi.server_to_cgi[1], &body_buffer[0], write_size);
			body_buffer.erase(body_buffer.begin(), body_buffer.begin() + write_size);
		}
		else
		{
			if (read_size + (ssize_t)body_buffer.size() > stringToLong(header["Content-Length"]))
				write_size = stringToLong(header["Content-Length"]) - read_size;
			else
				write_size = body_buffer.size();
			write(cgi.server_to_cgi[1], &body_buffer[0], write_size);
			body_buffer.clear();
		}
	}
	read_size += write_size;
	if (read_size == server.client_max_body_size || read_size == stringToLong(header["Content-Length"]))
	{
		close(cgi.server_to_cgi[1]);
		cgi.step++;
	}
}

int Client::waitForCGI(void)
{
	int status = -1;

	if (waitpid(cgi.pid, &status, WNOHANG) == 0)
		return (0);
	cgi.pid = 0;
	if (WEXITSTATUS(status) > 0)
		return (setupInternalServerError(server.error_pages));
	cgi.step++;
	return (0);
}

void Client::readFromCGI(void)
{
	char buffer[BUFFER_DATA + 1];
	int ret = 0;
	ret = read(cgi.cgi_to_server[0], buffer, BUFFER_DATA);
	if (!ret)
	{
		close(cgi.cgi_to_server[0]);
		std::string status = "Status: ";
		if (cgi.body_buffer.empty() || strncmp(cgi.body_buffer.data(), status.c_str(), status.size()))
		{
			setupInternalServerError(server.error_pages);
			return ;
		}
		cgi.step++;
		return ;
	}
	cgi.body_buffer.insert(cgi.body_buffer.end(), buffer, buffer + ret);
}

void Client::CGIResponse(void)
{
	if (cgi.body_buffer.empty())
	{
		action = REMOVE_CLIENT;
		return ;
	}
	if (!response.second_time)
	{
		cgi.body_buffer.erase(cgi.body_buffer.begin(), cgi.body_buffer.begin() + 9);
		std::string http = "HTTP/1.1 ";
		cgi.body_buffer.insert(cgi.body_buffer.begin(), http.begin(), http.end());
		response.second_time = true;
	}
	if (cgi.body_buffer.size() > BUFFER_DATA)
	{
		send(sock_fd, cgi.body_buffer.data(), BUFFER_DATA, 0);
		cgi.body_buffer.erase(cgi.body_buffer.begin(), cgi.body_buffer.begin() + BUFFER_DATA);
	}
	else
	{
		send(sock_fd, cgi.body_buffer.data(), cgi.body_buffer.size(), 0);
		cgi.body_buffer.clear();
	}
}

void Client::sendCGIResponse(void)
{
	switch (cgi.step)
	{
		case 0:
			executeCGI(); // fork and execve
			break ;
		case 1:
			sendBodyToCGI(); // send body to cgi
			break ;
		case 2:
			waitForCGI(); // wait for cgi
			break ;
		case 3:
			readFromCGI(); // read from cgi
			break ;
		case 4:
			CGIResponse(); // parse cgi response
			break ;
	}

}

void Client::sendDeleteResponse(void)
{
	unlink(response.filename.c_str());
	send(sock_fd, response.header.c_str(), response.header.size(), 0);
	action = REMOVE_CLIENT;
}

void Client::handleRequest(short port, std::vector<ServerBlock>& config)
{
	if (action == READ_SOCKET || action == REMOVE_CLIENT)
		return ;
	if (action == SETUP_RESPONSE)
		setupResponse(port, config);
	else if (action == REGULAR_RESPONSE || action == ERROR_RESPONSE)
		sendRegularResponse();
	else if (action == AUTOINDEX_RESPONSE || action == REDIRECTION_RESPONSE)
		sendAutoIndexAndRedirection();
	else if (action == UPLOAD_RESPONSE)
		sendUploadResponse();
	else if (action == CGI_RESPONSE)
		sendCGIResponse();
	else if (action == DELETE_RESPONSE)
		sendDeleteResponse();
}