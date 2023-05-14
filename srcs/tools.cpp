/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelidris <nelidris@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 18:44:14 by nelidris          #+#    #+#             */
/*   Updated: 2023/05/13 18:44:19 by nelidris         ###   ########.fr       */
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
	else if (extention == "png" || extention == "jpeg")
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

long int stringToLong(const std::string& str)
{
    std::istringstream iss(str);
    long int result;
    char remainder;
    if (!(iss >> result) || (iss >> remainder)) {
        throw (std::invalid_argument("stringTolong: invalid argument"));
    }
    return result;
}