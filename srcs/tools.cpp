/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelidris <nelidris@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 18:44:14 by nelidris          #+#    #+#             */
/*   Updated: 2023/06/08 14:33:49 by nelidris         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>

std::string removeWhiteSpace(std::string line)
{
    size_t start = 0;
    size_t end = line.size() - 1;

    while (isspace(line[start])) start++;
    while (end > 0 && isspace(line[end])) end--;

    return (line.substr(start, end - start + 1));
}

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

std::string getFileType(std::string filename)
{
	std::string::reverse_iterator dot = std::find(filename.rbegin(), filename.rend(), '.');
	if (dot == filename.rend())
		return ("text/plain");
	std::string extention = dot.base().base();
	if (extention == "html" || extention == "css" || extention == "json")
		return ("text/" + extention);
	else if (extention == "js")
		return ("text/javascript");
	else if (extention == "png" || extention == "jpeg" || extention == "jpg" || extention == "gif")
		return ("image/" + extention);
	else if (extention == "woff" || extention == "ttf")
		return ("font/" + extention);
	else if (extention == "mp4" || extention == "avi")
		return ("video/" + extention);
	else if (extention == "mp3" || extention == "mpeg")
		return ("audio/" + extention);
	else if (extention == "pdf")
		return ("application/" + extention);
	return ("text/plain");
}

bool is_directory(const char* path)
{
    struct stat st;
    if (stat(path, &st) != 0)
        return (false);
    if (!S_ISDIR(st.st_mode))
        return (false);
    return (true);
}

std::string stringify(long value)
{
    std::stringstream ss;
    ss << value;
    return ss.str();
}

long getFileSize(const std::string& filename)
{
    struct stat st;
    if (stat(filename.c_str(), &st) != 0)
        return (-1);
    return st.st_size;
}

std::string getCurrentTime()
{
    char buffer[100];
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = gmtime(&rawtime);

    strftime(buffer, 100, "%a, %d %b %Y %H:%M:%S GMT", timeinfo);

    return std::string(buffer);
}

bool emptyLine(const std::string& line)
{
	for (size_t i = 0; i < line.size(); i++)
	{
		if (!std::isspace(line[i]))
			return (false);
	}
	return (true);
}

std::vector<std::string> split(const std::string& str, char delim)
{
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;
    while ((end = str.find(delim, start)) != std::string::npos) {
        if (!emptyLine(str.substr(start, end - start)))
            tokens.push_back(str.substr(start, end - start));
        start = end + 1;
    }
    if (!emptyLine(str.substr(start)))
        tokens.push_back(str.substr(start));
    return tokens;
}

std::string convertToCGIHeader(std::string key)
{
    std::string result;
    if (key != "Content-Type" && key != "Content-Length")
        result += "HTTP_";
    for (size_t i = 0; i < key.size(); i++)
    {
        if (key[i] == '-')
            result += '_';
        else
            result += std::toupper(key[i]);
    }
    return (result);
}

ssize_t stringToLong(const std::string& str)
{
    std::istringstream iss(str);
    long int result;
    char remainder;
    if (!(iss >> result) || (iss >> remainder)) {
        throw (std::invalid_argument("stringTolong: invalid argument"));
    }
    return result;
}

void printReady(ServerBlock& config)
{
    char *host = reinterpret_cast<char*>(&config.listen.first);
    std::cout << "--> Server \033[1;34mhttp://" << (int)host[3] << '.' << (int)host[2] << '.' << (int)host[1] << '.' << (int)host[0] << ":" << config.listen.second << "/\033[0m is ready" << std::endl;
}