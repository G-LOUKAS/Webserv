#include "WebServer.hpp"
#include "../web_server/WebServer.hpp"
#include <stack>
#include <set>

WebServer::WebServer(ConfigFile& _config) : config(_config), max_fd(-1)
{
}

WebServer::~WebServer() 
{
    for (std::vector<int>::iterator it = server_socks_.begin(); it != server_socks_.end(); it++)
    {
        if (*it != -1) 
            close(*it);
    }
    for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++) 
        close(it->first);
}

void WebServer::setupServers() 
{
    std::vector<Server> servers = config.getServers();
    std::set<std::pair<std::string, int> > uniqueBindings;
    for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++) 
    {
        std::pair<std::string, int> bindKey = std::make_pair(it->host, it->port);
        if (uniqueBindings.find(bindKey) == uniqueBindings.end()) 
        {
            int server_sock = socket(AF_INET, SOCK_STREAM, 0);
            if (server_sock < 0)
                throw std::runtime_error("Failed to create server socket");  
            int opt = 1;
            if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
                throw std::runtime_error("Failed to set socket options");    
            struct sockaddr_in server_addr;
            memset(&server_addr, 0, sizeof(server_addr));
            server_addr.sin_family = AF_INET;
            server_addr.sin_addr.s_addr = inet_addr(it->host.c_str());
            server_addr.sin_port = htons(it->port); 
            if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
                throw std::runtime_error("Bind failed");
            if (listen(server_sock, 1000) < 0)
                throw std::runtime_error("Listen failed");
            server_socks_.push_back(server_sock);
            if (server_sock > max_fd)
                max_fd = server_sock;
            uniqueBindings.insert(bindKey);
        }
        else
            continue;
    }
}

void WebServer::acceptNewClient(int server_sock)
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int new_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_addr_len);
    if (new_sock < 0) 
        throw std::runtime_error("Accept failed");
    Client new_client(new_sock, config);
    new_client.setTime(time(NULL));
    if (clients.find(new_sock) == clients.end())
        clients.insert(std::make_pair(new_sock, new_client));
    if (new_sock > max_fd)
        max_fd = new_sock;
    FD_SET(new_sock, &readSet);
}

void WebServer::run()
{
    FD_ZERO(&readSet);
    FD_ZERO(&writeSet);
    while (true) 
    {
        timeout.tv_sec = 1; 
        timeout.tv_usec = 0;
        fd_set tmpReadSet = readSet;
        fd_set tmpWriteSet = writeSet;
        fd_set tmpErrorSet;
        FD_ZERO(&tmpErrorSet);
        for (std::vector<int>::iterator it = server_socks_.begin(); it != server_socks_.end(); it++)
            FD_SET(*it, &tmpReadSet);

        int nbrSelect = select(max_fd + 1, &tmpReadSet, &tmpWriteSet, &tmpErrorSet, &timeout);

        std::stack<std::map<int, Client>::iterator> needToRemove;
        if (nbrSelect < 0)
        {
            for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); it++)
            {
                if (FD_ISSET(it->first, &tmpErrorSet))
                {
                    FD_CLR(it->first, &readSet);
                    FD_CLR(it->first, &writeSet);
                    needToRemove.push(it);
                }
            }
            continue;
        }
        for (std::map<int, Client>::iterator it = clients.begin();it != clients.end(); it++) 
        {
            if (FD_ISSET(it->first, &tmpReadSet))  
            {
                try 
                {
                    it->second.setTime(time(NULL));
                    it->second.receive();
                    if (it->second.isCompleted()) 
                    {
                        FD_CLR(it->first, &readSet);
                        FD_SET(it->first, &writeSet);
                    }
                }
                catch (const std::exception &e) 
                {
                    std::cerr << "Error: " << e.what() << std::endl;
                    needToRemove.push(it);
                    continue;
                }
            }
            else if (FD_ISSET(it->first, &tmpWriteSet))
            {
                try 
                {
                    it->second.setTime(time(NULL));
                    it->second.send();
                    if (it->second.isClosed()) 
                    {
                        Files::removeFile(it->second.getcgiOutput());
                        Files::removeFile(it->second.getauto_file());
                        FD_CLR(it->first, &writeSet);
                        needToRemove.push(it);               
                    }
                } 
                catch (const std::exception &e) 
                {
                    std::cerr << "Error: " << e.what() << std::endl;
                    Files::removeFile(it->second.getcgiOutput());
                        Files::removeFile(it->second.getauto_file());
                    FD_CLR(it->first, &writeSet);
                    needToRemove.push(it);     
                    continue;
                }
            }
            else if (time(NULL) - it->second.getTime() > 10)
            {
                FD_CLR(it->first, &writeSet);
                FD_CLR(it->first, &readSet);
                needToRemove.push(it);
            }
        }

        for (std::vector<int>::iterator it = server_socks_.begin(); it != server_socks_.end(); it++) 
        {
            int server_sock = *it;
            if (FD_ISSET(server_sock, &tmpReadSet))
            {
                acceptNewClient(server_sock); 
            }
        }

        while (!needToRemove.empty())
        {
            Files::closeIFile(needToRemove.top()->first);
            Files::closeFile(needToRemove.top()->first);
            close(needToRemove.top()->first);
            clients.erase(needToRemove.top());
            needToRemove.pop();
        }

    }
}

