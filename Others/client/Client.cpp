/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kaoel-yo <kaoel-yo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 17:24:07 by kaoel-yo          #+#    #+#             */
/*   Updated: 2024/06/11 15:36:21 by kaoel-yo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
// *************************************************************************************** //
// ***                            Constructor & Destructor                             *** //
// *************************************************************************************** //
Client::Client(int sock, ConfigFile& _config) : _sock(sock), config(_config)
{
    writePos = 0;
    _connected = true; 
    _first = true;
    global_read = 0;
    _closed = false;
    _completed = false;
    header_received = false;
    body_received = false;
    _first_w = true;
    _fihead = true;
    from = false;
    auto_file = "";
    index_her = 0;
    request = HttpRequest();
    response = HttpResponse();
}

Client::~Client()
{
}

// *************************************************************************************** //
// ***                                 Member Functions                                *** //
// *************************************************************************************** //
void Client::receive()
{
    char buffer[1024] = {0};
    int valread;
    valread = read(_sock, buffer, sizeof(buffer) - 1);
    request.setFd(_sock);
    if (valread < 0)
        throw std::runtime_error("Failed to read from socket");
    else if (valread == 0)
    {
        Files::closeFile(_sock);
        _connected = false;
        return;
    }
    global_read += valread;
    request.setValRead(valread);
    request.setFirst(_first);
    if (_first)
    {
        request.parseRequestLine(buffer, config);
        if (request.is_Error())
        {
            Files::closeFile(_sock);
            _completed = true;
            return;
        }
    }
    if (!header_received)
        if (request.enoughHeaders(buffer))
            header_received = true;
    if (_fihead)
    {    
        request.parseHeaders(buffer, header_received);
        global_read -= request.getr_end();
    }
    if (request.is_Error())
    {
        Files::closeFile(_sock);
        _completed = true;
        return;
    }
    if (_fihead && header_received)
    {    
        _fihead = false;
        from = true;
    }
    else
        from = false;
    if (header_received)
    {
        if (request.getMethod() != 1)
        {
            Files::closeFile(_sock);
            _completed = true;
            return;
        }
        request.parseBody(buffer, from);
        if (request.is_Error())
        {
            Files::closeFile(_sock);
            _completed = true;
            return;
        }
        if (request.enoughtData(global_read))
            body_received = true;
        if (body_received)
        {
            Files::closeFile(_sock);
            _completed = true;
        }
    }
    _first = false;
}

void Client::send()
{
    bool here = false;
    if (_completed)
    {
        if (_first_w)
        {
            response.setRequest(request, _sock);
            response.setConfigInfos();
            if (!response.keepAlive())
                _connected = false;
            response.responseProcess(o_cgi);
            writePos = 0;
            setauto_file(response.getauto_file());
        }
        if (response.isEnd())
        {
            if (!response.getCheckCgi())
                response.responseProcess(o_cgi);
            else
            {
                _closed = response.createResponse(_first_w);
                global_write = response.getGlobalRead();
                std::string response_str = response.getResponse();
                long int valwrite = 0;
                valwrite = write(_sock, response_str.c_str(), global_write);
                if (valwrite < 0)
                    throw std::runtime_error("Failed to write to socket");
                else if (valwrite == 0)
                    response.setIsZero(true);
                else
                {
                    here = true;
                    response.setIsZero(false);
                    response.setResponse();
                }
                if (_first_w && here)
                    _first_w = false;
            }
        }
    }
}

int Client::getSock()const
{
    return (_sock);
}

bool Client::isCompleted()
{
    return _completed;
}

bool Client::isClosed()
{
    return _closed;
}

void Client::setTime(time_t t)
{
_time = t;
}

time_t Client::getTime() const
{
return (_time);
}

std::string Client::getauto_file()
{
    return(auto_file);
}

void Client::setauto_file(std::string name)
{
    auto_file = name;
}

std::string Client::getcgiOutput()
{
    return (o_cgi.getCgiOutput());
}