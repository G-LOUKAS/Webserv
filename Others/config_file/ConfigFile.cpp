/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configFile.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaoel-yo <kaoel-yo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/27 14:00:18 by kaoel-yo          #+#    #+#             */
/*   Updated: 2024/06/08 17:51:40 by kaoel-yo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigFile.hpp"

ConfigFile::ConfigFile()
{
}

ConfigFile::~ConfigFile()
{
}

ConfigFile &ConfigFile::operator=(const ConfigFile &other)
{
    if (this != &other)
    {
        this->fileName = other.fileName;
        this->servers = other.servers;
    }
    return (*this);
}

ConfigFile::ConfigFile(const ConfigFile &other)
{
    *this = other;
}

ConfigFile::ConfigFile(std::string _fileName): fileName(_fileName), brace(SERVER_CLOSED)
{
    file.open(fileName.c_str());
    if (!file.is_open())
    {
        throw InvalidSyntax("error: opening file");
        file.close();
        return;
    }
}

std::string ConfigFile::getFileName() const
{
    return (fileName);
}

std::string ConfigFile::trimString(std::string& line)
{
    if (line.empty())
        return line;
    std::size_t start = 0;
    while (start < line.size() && std::isspace(line[start]))
        start++;
    std::size_t end = line.size() - 1;
    while (end > start && std::isspace(line[end]))
        end++;
    return (line.substr(start, end - start + 1));
}

std::vector<std::string> ConfigFile::Spliting(std::string& line)
{
    std::vector<std::string> words;
    std::stringstream ss(line);
    std::string word;

    while (ss >> word)
        words.push_back(word);
    return words;
}

void ConfigFile:: Convert_ip_address_to_IPV4_format(std::string& ipString)
{
    struct in_addr ipv4Addr;
    // Convert IP address string to IPv4 format
    if (inet_pton(AF_INET, ipString.c_str(), &ipv4Addr) != 1)
    {
        // Conversion failed
       throw InvalidSyntax("invalid host");
    }
}

void ConfigFile::ParsConfigFile()
{
    std::string line;
    std::vector<std::string> words;
    Server server;
    Location location;

    while(getline(file, line))
    {
        line = trimString(line);
        if (line.empty() || line[0] == '#')
            continue;
        words = Spliting(line);
        if (brace == SERVER_CLOSED && words[0] != "server")
            throw InvalidSyntax("error: invalid server");
        if (words[0] == "server")
        {
            if(words.size() != 2 || words[1] != "{" || brace != SERVER_CLOSED)
                throw InvalidSyntax("error: Invalid server");
            brace = INSIDE_SERVER;
            resetServer(server);
        }
        else if (words[0] == "listen")
        {
            if (brace == INSIDE_SERVER)
            {
                if (words.size() != 3 || !server.host.empty() )
                    throw InvalidSyntax("invalid listen");
                Convert_ip_address_to_IPV4_format(words[1]);
                server.host = words[1];
                for (size_t i = 0; i < words[2].size() ; i++)
                {
                    if (!isdigit(words[2][i]))
                        throw InvalidSyntax("invalid port");
                }
                server.port = _stoi(words[2]);
                if (server.port == 0 || server.port > 65536)
                    throw std::out_of_range("Port number out of range");
            }
            else
                throw InvalidSyntax("Error: listen must be inside server block");
                
        }
        else if (words[0] == "server_name")
        {
            if (brace == INSIDE_SERVER)
            {
                if (words.size() != 2 || !server.serverName.empty())
                    throw InvalidSyntax("invalid server name");
                server.serverName = words[1];
            }
            else
                throw InvalidSyntax("Error: server_name directive must be inside server block");
        }
        else if (words[0] == "index")
        {
            if ((!server.index.empty() && brace == INSIDE_SERVER) || (!location.index.empty() && brace == INSIDE_LOCATION))
                throw InvalidSyntax("invalid index");
            else if (brace == INSIDE_SERVER)
            {
                for (size_t i = 1; i < words.size(); i++)
                    server.index.push_back(words[i]);
            }
            else
            {
                for (size_t i = 1; i < words.size(); i++)
                    location.index.push_back(words[i]);
            }

        }
        else if (words[0] == "root")
        {
            if ((words.size() != 2 || words[1][0] != '/') || (!server.root.empty() && brace == INSIDE_SERVER)|| (!location.root.empty() && brace == INSIDE_LOCATION))
                throw InvalidSyntax("Invalid root");
            else if (brace == INSIDE_SERVER)
                server.root = words[1];
            else
                location.root = words[1];
        }
        else if (words[0] == "error_page")
        {
            if (brace == INSIDE_SERVER)
            {
                if (words.size() != 3)
                    throw InvalidSyntax("invalid error_page");
                int key = _stoi(words[1]);
                server.errorPage[key] = words[2];
            }
            else
                throw InvalidSyntax("Error: error_page directive must be inside server block");
        }
        else if (words[0] == "client_max_body_size")
        {
            if (brace == INSIDE_SERVER)
            {
                if (words.size() != 2 || server.max_body_size != 0)
                    throw InvalidSyntax("invalid size");
                for (size_t i = 0 ; i < words[1].size() ; i++)
                    if (!isdigit(words[1][i]))
                        throw InvalidSyntax("invalid size");
                server.max_body_size = _stoi(words[1]);
            }
            else
                throw InvalidSyntax("Error: client_max_body_size directive must be inside server block");
        }
        else if (words[0] == "location")
        {
            if (words.size() != 3 || words[2] != "{" || server.root.empty())
                throw InvalidSyntax("invalid location");
            if (words[1][0] != '/')
                throw InvalidSyntax("invalid uri");
            resetLocation(location);
            location.uri = words[1];
            brace = INSIDE_LOCATION;
        }
        else if (words[0] == "cgi")
        {
            if ( brace == INSIDE_LOCATION)
            {
                if (words.size() < 2 || !location.cgi.empty())
                    throw InvalidSyntax("invalid cgi");
                for (size_t i = 1; i < words.size(); i++)
                {
                    if(words[i] != ".py" && words[i] != ".php")
                    {
                        throw InvalidSyntax("invalid cgi"); 
                    }
                    location.cgi.push_back(words[i]);
                }
            }
            else
                throw InvalidSyntax("Error: CGI directive must be inside location block");
        }
        else if (words[0] == "allow_methods")
        {
            if (brace == INSIDE_LOCATION)
            {
                if (!location.allowMethods.empty())
                    throw InvalidSyntax("invalid method");
                for (size_t i = 1; i < words.size(); i++)
                {
                    if (words[i] != "GET" && words[i] != "POST" && words[i] != "DELETE")
                    {
                        throw InvalidSyntax("invalid method");
                    }
                    location.allowMethods.push_back(words[i]);
                }
            }
            else
                throw InvalidSyntax("Error: allow_methods directive must be inside server block");
        }
        else if (words[0] == "upload")
        {
            if (brace == INSIDE_LOCATION)
            {
                if (words.size() != 2)
                    throw InvalidSyntax("invalid upload");
                location.upload = words[1];
                if (location.upload[0] != '/' || location.upload[location.upload.length()-1] != '/')
                    throw InvalidSyntax("invalid upload2");
            }
            else
                throw InvalidSyntax("Error: upload directive must be inside location block");
        }

        else if (words[0] == "rewrite")
        {
            if (brace == INSIDE_LOCATION)
            {
                if (words.size() != 2)
                    throw InvalidSyntax("invalid redirection");
                location.redirect= words[1];
                if (location.redirect[0] != '/' || location.redirect[location.redirect.length()-1] != '/')
                    throw InvalidSyntax("invalid redirection 2");
            }
            else
                throw InvalidSyntax("Error: rewrite directive must be inside location block");
        }
        else if (words[0] == "autoindex")
        {
            if (words.size() != 2 || (words[1] != "on" && words[1] != "off" ))
                throw InvalidSyntax("invalid autoindex");
            if (words[1] == "on" && brace == INSIDE_LOCATION)
                location.autoindex = true;
            if (words[1] == "on" && brace == INSIDE_SERVER)
                server.autoindex = true;
        }
        else if (words[0] == "}")
        {
            if (brace == INSIDE_LOCATION)
            {
                brace = LOCATION_CLOSED;
                setDefaultBehavior(location,server);
                server.locations.push_back(location);
            }
            else if (brace == LOCATION_CLOSED || brace == INSIDE_SERVER)
            {
                brace = SERVER_CLOSED;
                if (server.host.empty() || server.port == -1 || server.serverName.empty())
                    throw InvalidSyntax("invalid server");                
                setDefaultBehavior(server);
                servers.push_back(server);
            }
            else
                throw InvalidSyntax("invalid syntax");
        }
        else
            throw InvalidSyntax("invalid syntax");
    }
    if (brace != SERVER_CLOSED)
        throw InvalidSyntax("invalid syntax");
    file.close();
}
std::vector<Server> ConfigFile::getServers() const
{
    return(servers);
}

void resetLocation(Location &location)
{
    location.uri.clear();
    location.root.clear();
    location.index.clear();
    location.allowMethods.clear();
    location.upload.clear();
    location.cgi.clear();
    location.redirect.clear();
    location.autoindex = false;
}

void resetServer(Server &server)
{
    server.host.clear();
    server.port = -1;
    server.serverName.clear();
    server.index.clear();
    server.root.clear();
    server.max_body_size = 0;
    server.errorPage.clear();
    server.locations.clear();
    server.autoindex = false;
}

void setDefaultBehavior(Server& server)
{
    if (server.index.empty())
        server.index.push_back("index.html");
    if (server.max_body_size == 0)
        server.max_body_size = 1048576; // 1 MB in bytes; 
}
void setDefaultBehavior(Location& location, Server& server)
{
    if (location.index.empty())
        location.index.push_back("index.html");
    if (location.allowMethods.empty())
    {
        location.allowMethods.push_back("GET");
        location.allowMethods.push_back("POST");
    }
    if (location.root.empty())
        location.root = server.root;
}

size_t ConfigFile::_stoi(std::string str)
{
    size_t i = 0;
    for (size_t j = 0; j < str.size(); j++)
    {
        if (!isdigit(str[j]))
            throw InvalidSyntax("Invalid number");
        i = i * 10 + str[j] - '0';
    }
    return i;
}

std::string ConfigFile::to_Str(int nb)
{
    std::stringstream ss;
    ss << nb;
    return ss.str();
}

void ConfigFile::check_servers(const std::vector<Server>& servers)
{
    if (servers.empty()) 
        throw InvalidSyntax( "No server configuration found");
    else
    {
        for (size_t i = 0; i < servers.size(); i++) 
        {
            for (size_t j = i + 1; j < servers.size(); j++) 
            {  
                if (servers[i].host == servers[j].host && servers[i].port == servers[j].port)
                {
                    if (servers[i].serverName == servers[j].serverName)
                        throw InvalidSyntax("Binding conflicts detected.");
                }
            }
        }
    }
}