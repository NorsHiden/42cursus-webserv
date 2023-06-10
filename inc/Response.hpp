/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelidris <nelidris@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/13 18:44:32 by nelidris          #+#    #+#             */
/*   Updated: 2023/06/10 21:16:02 by nelidris         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __RESPONSE_HPP__
# define __RESPONSE_HPP__

# include <webserv.hpp>

struct Response
{
	int	body_fd;
	std::string header;
	std::string autoindex_body;
	int upload_fd;
	bool second_time;
	std::string filename;

	Response(): body_fd(-1), upload_fd(-1), second_time(false) {}
	~Response()
	{
		if (body_fd > 0) close(body_fd);
		if (upload_fd > 0) close(upload_fd);
	}
};

#endif /* __RESPONSE_HPP__ */