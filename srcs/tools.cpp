# include <webserv.hpp>

bool emptyLine(const std::string& line)
{
	for (size_t i = 0; i < line.size(); i++)
	{
		if (!std::isspace(line[i]))
			return (false);
	}
	return (true);
}

// std::vector<std::string> split(const std::string& str, char delim)
// {
//     std::vector<std::string> tokens;
//     std::stringstream ss(str);
//     std::string token;

//     // check if the delimiter is present in the input string
//     if (str.find(delim) == std::string::npos) {
//         tokens.push_back(str);
//         return tokens;
//     }

//     while (std::getline(ss, token, delim)) {
//         if (!emptyLine(token))
//             tokens.push_back(token);
//     }
//     for (size_t i = 0; i < tokens.size(); i++)
//         std::cout << tokens[i] << "->" << tokens[i].size() << std::endl;
//     return tokens;
// }

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
    
    for (size_t i = 0; i < tokens.size(); i++)
        std::cout << tokens[i] << "->" << tokens[i].size() << std::endl;
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