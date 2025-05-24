/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaoel-yo <kaoel-yo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/10 14:52:18 by kaoel-yo          #+#    #+#             */
/*   Updated: 2024/06/11 15:50:14 by kaoel-yo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */



#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <iostream>
#include <cstring>
#include <vector>
#include <stdexcept>
#include <sys/select.h>
#include <sys/time.h>
#include <algorithm>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <unistd.h>
#include <map>
#include "../client/Client.hpp"
#include "../config_file/ConfigFile.hpp"
#include "../request/HttpRequest.hpp"
#include "../response/HttpResponse.hpp"

// include CLient class

class Client;

class WebServer 
{
public:
    WebServer(ConfigFile& _config);
    ~WebServer();
    void setupServers();
    void acceptNewClient(int server_sock);
    void run();
  
private:
    std::vector<int> server_socks_;
    ConfigFile& config;
    std::map<int, Client> clients; 
    fd_set readSet;
    fd_set writeSet;
    int max_fd;
    struct timeval timeout;
};



#endif
