/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaoel-yo <kaoel-yo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 17:45:43 by kaoel-yo          #+#    #+#             */
/*   Updated: 2024/06/10 15:14:30 by kaoel-yo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include "../config_file/ConfigFile.hpp"
#include "../request/HttpRequest.hpp"
#include "../response/HttpResponse.hpp"
#include "../extentions/Ext.hpp"
#include "../codes/Codes.hpp"
#include "../cgi/CGI.hpp"
#include "../web_server/WebServer.hpp"
#include "../files/Files.hpp"

class CGI;
class Client
{
    private:
    // ****************************************************** //
    // ***                 Data Members                   *** //
    // ****************************************************** //
    int _sock;
    size_t writePos;
    bool _connected;
    bool _first;
    bool _closed;
    bool _completed;
    bool header_received;
    bool _fihead;
    int index_her;
    bool from;
    bool body_received;
    bool _first_w;
    long int global_read;
    long int global_write;
    ConfigFile config;
    CGI o_cgi;
    HttpRequest request;
    HttpResponse response;
    time_t _time;
    std::string auto_file;
    // ****************************************************** //

public:
    // ****************************************************** //
    // ***              Constructor & Destructor          *** //
    // ****************************************************** //
    Client(int _sock, ConfigFile& _config);
    ~Client();
    // ****************************************************** //
    // ***                 Public Functions               *** //
    // ****************************************************** //
    void receive();
    void send();
    int getSock()const;
    bool isCompleted();
    bool isClosed();
    void setTime(time_t t);
    time_t getTime() const;
    std::string getauto_file();
    void setauto_file(std::string name);
    std::string getcgiOutput();
    // ****************************************************** //
};

#endif
