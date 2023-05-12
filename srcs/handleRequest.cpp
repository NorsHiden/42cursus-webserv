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

void Client::parseRequestHeader(void)
{
	std::stringstream in(header_buffer);
	std::string line;

	std::getline(in, line);
	start_line = split(line, ' ');
	while (std::getline(in, line))
		parseHeaderLine(line);
}

void Client::readRequest(void)
{
	char buffer[BUFFER_DATA];
	memset(buffer, 0, sizeof(buffer));
	int ret = recv(sock_fd, buffer, BUFFER_DATA, 0);

	if (ret <= 0)
	{	
		std::cout << "connection closed || error reading from sock_fd" << std::endl;
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
			if (!request_buffer.empty())
				appendToBody(&body, body_size, buffer + pos + 4, ret - pos - 4);
			parseRequestHeader();
			action = SETUP_RESPONSE;
		}
	}
	else
		appendToBody(&body, body_size, buffer, ret);
}


void Client::sendRegularResponse(void)
{
	if (!response.second_time)
	{
		send(sock_fd, response.header.c_str(), response.header.size(), 0);
		response.second_time = true;
	}
	else
	{
		char buffer[BUFFER_DATA];
		ssize_t n = read(response.body_fd, buffer, BUFFER_DATA);
		if (!n)
			action = REMOVE_CLIENT;
		else
			send(sock_fd, buffer, n, 0);
	}
}

void Client::sendAutoIndexAndRedirection(void)
{
	send(sock_fd, response.header.c_str(), response.header.size(), 0);
	if (action == AUTOINDEX_RESPONSE)
		send(sock_fd, response.autoindex_body.c_str(), response.autoindex_body.size(), 0);
	action = REMOVE_CLIENT;
}

void Client::handleRequest(std::vector<ServerBlock>& config)
{
	if (action == READ_SOCKET || action == REMOVE_CLIENT)
		return ;
	if (action == SETUP_RESPONSE)
		setupResponse(config);
	else if (action == REGULAR_RESPONSE || action == ERROR_RESPONSE)
		sendRegularResponse();
	else if (action == AUTOINDEX_RESPONSE || action == REDIRECTION_RESPONSE)
		sendAutoIndexAndRedirection();
}