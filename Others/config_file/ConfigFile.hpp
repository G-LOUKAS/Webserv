/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configFile.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaoel-yo <kaoel-yo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 14:00:11 by kaoel-yo          #+#    #+#             */
/*   Updated: 2024/05/30 16:24:21 by kaoel-yo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CONFIGFILE_HPP
#define CONFIGFILE_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <cstdlib>
#include <exception>
#include <sstream>
#include <arpa/inet.h>
#include <algorithm>

#define SERVER_CLOSED 0
#define LOCATION_CLOSED 1
#define INSIDE_SERVER 2
#define INSIDE_LOCATION 3

typedef struct s_loction
{
    std::string uri;
    std::string root;
    std::vector<std::string> index;
    std::vector <std::string> allowMethods;
    std::string upload;
    std::vector<std::string> cgi;
    std::string redirect;
    bool    autoindex;
}   Location;

typedef struct s_Server
{
    std::string host;
    int    port;
    std::string serverName;
    std::vector<std::string>    index;
    std::string root;
    long long int   max_body_size;
    std::map<int, std::string>  errorPage;
    std::vector<Location> locations;
    bool    autoindex;
}   Server;

void resetLocation(Location &location);
void resetServer(Server &server);
void setDefaultBehavior(Server& server);
void setDefaultBehavior(Location& location, Server& server);

class ConfigFile
{
    private:
        std::ifstream file;
        std::string fileName;
        int brace;
        std::vector <Server> servers;
        ConfigFile(); // unused private constructer
    public:
        ~ConfigFile();
        ConfigFile(std::string _fileName);
        /******operator overloading ***/
        ConfigFile& operator=(const ConfigFile& other);
        ConfigFile(const ConfigFile& other);
        /**************************** */
        std::string getFileName() const;
        std::vector<std::string> Spliting(std::string& line);
        std::string trimString(std::string& line);
        void Convert_ip_address_to_IPV4_format(std::string& ipString);
        std::vector<Server> getServers() const; // Getter for servers
        void ParsConfigFile();
        class InvalidSyntax : public std::exception
        {
            private:
                const char* message;
            public:
                InvalidSyntax(const char * message) : message(message){}
                const char* what() const throw(){
                    return message;
                }
        };
        size_t _stoi(std::string str);
        std::string to_Str(int nb);
        void check_servers(const std::vector <Server>& servers);
};


#endif