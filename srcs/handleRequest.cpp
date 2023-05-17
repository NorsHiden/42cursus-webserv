/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleRequest.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelidris <nelidris@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 18:43:40 by nelidris          #+#    #+#             */
/*   Updated: 2023/05/16 14:55:56 by nelidris         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>

void	appendToBody(char **body, size_t &body_size, char *buffer, size_t buffer_size)
{
	if (buffer_size == 0)
		return ;
	char *tmp = new char[body_size + buffer_size];
	for (size_t i = 0; i < body_size; i++)
		tmp[i] = (*body)[i];
	for (size_t i = 0; i < buffer_size; i++)
		tmp[body_size + i] = buffer[i];
	if (body_size > 0)
		delete[] *body;
	*body = tmp;
	body_size += buffer_size;
}

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
	for (; chunked_body.pos_body < body_size - 1; chunked_body.pos_body++)
	{
		if (body[chunked_body.pos_body] == '\r' && body[chunked_body.pos_body + 1] == '\n')
		{
			if (chunked_body.looking_for_size)
			{
				body[chunked_body.pos_body] = 0;
				chunked_body.grabbed_size = strtol(body + chunked_body.start_body, NULL, 16);
				if (chunked_body.grabbed_size == 0)
				{
					chunked_body.reading_done = true;
					break ;
				}
				chunked_body.start_body = chunked_body.pos_body + 2;
				chunked_body.looking_for_size = false;
			}
			else if (chunked_body.pos_body - chunked_body.start_body >= chunked_body.grabbed_size)
			{
				chunked_body.looking_for_size = true;
				appendToBody(&chunked_body.buffer, chunked_body.size, body + chunked_body.start_body, chunked_body.grabbed_size);
				chunked_body.start_body = chunked_body.pos_body + 2;
				chunked_body.grabbed_size = 0;
			}
		}
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
				appendToBody(&body, body_size, buffer + pos + 4, ret - pos - 4);
				if (chunked_body.chunked_body && !chunked_body.reading_done)
					adjustBody();
			}
			action = SETUP_RESPONSE;
		}
	}
	else
	{
		appendToBody(&body, body_size, buffer, ret);
		if (chunked_body.chunked_body && !chunked_body.reading_done)
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
		int sent_bytes = 0; // for debugging;
		int pos = 0;
		while (pos < n)
		{
			sent_bytes = send(sock_fd, buffer + pos, n - pos, 0);
			if (sent_bytes < 0)
			{
				lseek(response.body_fd, -(n - pos), SEEK_CUR);
				return ;
			}
			pos += sent_bytes;
		}
	}
}

void Client::sendAutoIndexAndRedirection(void)
{
	if (send(sock_fd, response.header.c_str(), response.header.size(), 0) < 0)
		action = REMOVE_CLIENT;
	if (action == AUTOINDEX_RESPONSE)
		send(sock_fd, response.autoindex_body.c_str(), response.autoindex_body.size(), 0);
	action = REMOVE_CLIENT;
}

void Client::chunkedUpload(void)
{
	if (pos + BUFFER_DATA > server.client_max_body_size)
	{
		if (chunked_body.size - pos > BUFFER_DATA)
		{
			write(response.upload_fd, chunked_body.buffer + pos, BUFFER_DATA);
			pos += BUFFER_DATA;
		}
		else
		{
			write(response.upload_fd, chunked_body.buffer + pos, chunked_body.size - pos);
			pos += chunked_body.size - pos;
		}
	}
	else if (chunked_body.size - pos > server.client_max_body_size)
	{
		write(response.upload_fd, chunked_body.buffer + pos, server.client_max_body_size - pos);
		pos += server.client_max_body_size - pos;
	}
	else
	{
		write(response.upload_fd, chunked_body.buffer + pos, chunked_body.size - pos);
		pos += chunked_body.size - pos;
	}
	if (pos == server.client_max_body_size || (chunked_body.reading_done && pos == chunked_body.size))
	{
		close(response.upload_fd);
		action = REMOVE_CLIENT;
	}
}

void Client::normalUpload(void)
{
	if (pos + BUFFER_DATA > (size_t)stringToLong(header["Content-Length"]))
	{
		if (body_size - pos > BUFFER_DATA)
		{
			write(response.upload_fd, body + pos, BUFFER_DATA);
			pos += BUFFER_DATA;
		}
		else
		{
			write(response.upload_fd, body + pos, body_size - pos);
			pos += body_size - pos;
		}
	}
	else if (body_size - pos > (size_t)stringToLong(header["Content-Length"]))
	{
		write(response.upload_fd, body + pos, stringToLong(header["Content-Length"]) - pos);
		pos += stringToLong(header["Content-Length"]) - pos;
	}
	else
	{
		write(response.upload_fd, body + pos, body_size - pos);
		pos += body_size - pos;
	}
	if (pos == (size_t)stringToLong(header["Content-Length"]))
	{
		send(sock_fd, response.header.c_str(), response.header.size(), 0);
		close(response.upload_fd);
		action = REMOVE_CLIENT;
	}
}

void Client::sendUploadResponse(void)
{
	if (!response.second_time)
	{
		send(sock_fd, response.header.c_str(), response.header.size(), 0);
		response.second_time = true;
		return ;
	}
	if (chunked_body.chunked_body)
		chunkedUpload();
	else
		normalUpload();
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
}