#include "HttpRequest.hpp"
// *************************************************************************************** //
// ***                            Constructor & Destructor                             *** //
// *************************************************************************************** //
HttpRequest::HttpRequest()
{
    is_error = false;
    error = 0;
    _first = true;
    pos = 0;
    query = "";
    connection = false;
    body.chunked = false;
    body.content_length = false;
    location_found = false;
    newcreated = false;
    cookie = false;
    valread = 0;
    pos = 0;
    index = 0;
    r_end = 0;

    flagMemory = 0;
    chunks_size = 0;        // the sum of all chunks

    size_dec = 0;           // the size of the chunk in decimal
    last_chunk = false;         // the size needed to finish the last chunk
    last = 0;                // the last position of the last size not finished yet

    salat = false;
}

HttpRequest::~HttpRequest()
{
}

// *************************************************************************************** //
// ***                                 Member Functions                                *** //
// *************************************************************************************** //
// REQUEST LINE
void HttpRequest::parseRequestLine(char *buffer, ConfigFile &config)
{
    if (buffer == NULL)
        return(IsError(400));
    for (int i = 0; i < valread; i++)
    {
        if (buffer[i] == '\r' && buffer[i + 1] == '\n')
        {
            pos = i;
            break;
        }
    }
    if (pos == 0)
        return(IsError(400));
    r_end = pos + 2;
    std::string tmp;
    tmp.append(buffer, pos);
    std::string line = tmp.substr(0, pos); 
    std::string method = line.substr(0, line.find(" "));
    line = line.substr(line.find(" ") + 1);
    if (line.find(" ") == std::string::npos)
        return(IsError(400));
    std::string target = line.substr(0, line.find(" "));
    line = line.substr(line.find(" ") + 1);
    std::string version = line.substr(0, line.find(" "));
    parseMethod(method);
    if (is_error)
        return;
    parseTarget(target);
    if (is_error)
        return;
    parseVersion(version);
    if (is_error)
        return;
    setConfig(config);
}

void HttpRequest::parseMethod(std::string _method)
{
    if (_method.empty())
        return(IsError(400));
    if (_method.compare("GET") == 0)
        requestLine.method = GET;
    else if (_method.compare("POST") == 0)
        requestLine.method = POST;
    else if (_method.compare("DELETE") == 0)
        requestLine.method = DELETE;
    else
        return(IsError(405));
}

void HttpRequest::parseTarget(std::string _target)
{
    if (_target.empty())
        return(IsError(400));
    for (size_t i = 0; i < _target.length(); i++)
    {
        if (_target[i] == '/')
        {
            if (i + 1 < _target.length() && _target[i + 1] == '/')
                return(IsError(400));
            if (i + 1 < _target.length() && _target[i + 1] == '.')
            {
                i++;
                if (i + 1 < _target.length() && _target[i + 1] == '.')
                {
                    i++;
                    if (i + 1 < _target.length() && _target[i + 1] == '/')
                        return(IsError(400));
                    else if (i + 1 == _target.length())
                        return(IsError(400));
                }
            }
        }
    }
    requestLine.uri = _target;
    res_uri = _target;
}

void HttpRequest::parseVersion(std::string _version)
{
    if (_version.empty())
        return(IsError(400));
    if (_version.find("HTTP/") != 0)
        return(IsError(400));
    std::string token = _version.substr(5);
    if (token.find('.') != std::string::npos)
    {
        std::string major = token.substr(0, token.find('.'));
        if (major.length() != 1)
            return(IsError(400));
        if (major.compare("1") != 0)
            return(IsError(505));
        token = token.substr(token.find('.') + 1);
        if (token.length() != 1)
            return(IsError(400));
        std::string minor = token.substr(0, 1);
        if (minor.compare("1") != 0)
            return(IsError(505));
    }
    else
    {
        if (token.length() != 1)
            return(IsError(400));
        else if (token.compare("1") != 0)
            return(IsError(505));
    }
    requestLine.version = _version;
}

// HEADERS
void HttpRequest::parseHeaders(char *buffer, bool headers_complete)
{
    if (valread == 0)
        return(IsError(400));
    int e;
    if (_first)
        e = r_end;
    else
        e = 0;
    while (e < valread)
    {
        tmp[index] = buffer[e];
        if (index > 9000)
            return(IsError(400));
        if (index >= 4 && tmp[index] == '\n')
        {
            if ((index - 1) > 0 && tmp[index - 1] == '\r')
            {
                if ((index - 2) > 0 && tmp[index - 2] == '\n')
                {
                    if ((index - 3) > 0 && tmp[index - 3] == '\r')
                    {
                        headers_complete = true;
                        break;
                    }
                }
            }
        }
        e++;
        index++;
    }
    r_end = e;
    if (!headers_complete)
        return;
    else
    {
        index++;
        tmp[index] = '\0';
        index = e + 1;
    }
    r_end = e;
    std::string headerss;
    headerss.append(tmp);
    size_t i;
    std::string header;
    while ((i = headerss.find("\r\n")) != std::string::npos)
    {
        header.clear();
        header.append(headerss.substr(0, i));
        headerss= headerss.substr(i + 2);
        if (header.empty() && headerss.compare("\r\n"))
            break;
        parseHeader(header);
        if (is_error)
            return;
    }
    if (cookie)
    {
        std::string cookie = headers["Cookie"];
        while ((i = cookie.find("; ")) != std::string::npos)
        {
            std::string tmp = cookie.substr(0, i);
            if (tmp.find("=") != std::string::npos)
            {
                std::string key = tmp.substr(0, tmp.find("="));
                if (key.empty())
                    return(IsError(400));
                if (tmp.find("=") + 1 < tmp.length())
                {
                    std::string value = tmp.substr(tmp.find("=") + 1);
                    cookie_args[key] = value;
                }
                else
                    cookie_args[key] = "";
            }
            if (i + 2 < cookie.length())
                cookie = cookie.substr(i + 2);
        }
        if (cookie.find("=") != std::string::npos)
        {
            std::string key = cookie.substr(0, cookie.find("="));
            std::string value = cookie.substr(cookie.find("=") + 1);
            cookie_args[key] = value;
        }
    }
    if (requestLine.method != 1)
        return;
    if (body.content_length && body.chunked)
        return(IsError(400));
    if (!body.content_length && !body.chunked)
        return(IsError(400));
    if (!getContentType().empty())
    {
        std::string content_type = getContentType();
        std::transform(content_type.begin(), content_type.end(), content_type.begin(), ::tolower);
        if (content_type.find("multipart/form-data") != std::string::npos || content_type.find("application/x-www-form-urlencoded") != std::string::npos)
        {
            if (!body.chunked && getContentLenght() == 0)
                return(IsError(400));
            createFile();
            return;
        }
    }
    std::string name;
    body.file_extension = ext.get_extension(getContentType());
    if (upload_path.empty())
    {
        if (path[path.length() - 1] == '/')
            name = path + "file" + body.file_extension;
        else
            name = path + "/file" + body.file_extension;
    }
    else
        name = upload_path + "file" + body.file_extension;
    std::ifstream checkFile(name.c_str());
    if (checkFile.good())
        checkFile.close();
    else
    {
        body.file_name = name;
        checkFile.close();
        return;
    }
    for (int i = 1; i < 1000; i++)
    {
        if (upload_path.empty())
        {
            if (path[path.length() - 1] == '/')
                name = path + "file" + _to_string(i) + body.file_extension;
            else
                name = path + "/file" + _to_string(i) + body.file_extension;
        }
        else
            name = upload_path + "file" + _to_string(i) + body.file_extension;
        std::ifstream checkFile(name.c_str());
        if (checkFile.good())
            checkFile.close();
        else
        {
            body.file_name = name;
            checkFile.close();
            break;
        }
    }
}

void HttpRequest::parseHeader(std::string header)
{
    std::string field_name = header.substr(0, header.find(":"));

    if (field_name.empty())
        return(IsError(400));
    std::string field_value = header.substr(header.find(":"));
    if (field_value.empty() || field_value.length() <= 2)
        return(IsError(400));
    if (field_value[0] != ':' || field_value[1] != ' ')
        return(IsError(400));
    field_value = field_value.substr(2, field_value.length() - 1);
    if (field_value.empty())
        return(IsError(400));
    if (headers.find(field_name) != headers.end())
        return(IsError(400));
    if (field_name.compare("Content-Length") == 0)
    {
        body.content_length = true;
        for (size_t i = 0; i < field_value.length(); i++) 
            if (!isdigit(field_value[i]))
                return(IsError(400));
        if (getContentLenght() < 0)
            return(IsError(400));
    }
    if (field_name.compare("Transfer-Encoding") == 0)
    {
        std::transform(field_value.begin(), field_value.end(), field_value.begin(), ::tolower);
        if (field_value.compare("chunked") == 0)
            body.chunked = true;
        else
            return(IsError(400));
    }
    if (field_name.compare("Connection") == 0)
    {
        std::transform(field_value.begin(), field_value.end(), field_value.begin(), ::tolower);
        if (field_value.compare("keep-alive") == 0)
            connection = true;
        else if (field_value.compare("close") != 0)
            return(IsError(400));
    }
    if (field_name.compare("Cookie") == 0)
            cookie = true;
    headers[field_name] = field_value;
}

// BODY
void HttpRequest::parseBody(char *buffer, bool from)
{
    std::string name;

    name = body.file_name;
    if (max_body_size - (valread - index) < 0)
    {
        Files::closeFile(fd);
        Files::removeFile(name);
        return(IsError(413));
    }
    if (!newcreated)
    {
        if (!from)
            index = 0;
        if (Files::openFile(fd, name) < 0)
            return(IsError(403));
        if (body.chunked)
            parseChunkedBody(buffer);
        else if (body.content_length)
            Files::writeFile(fd, buffer + index, valread - index);
        newcreated = true;
    }
    else
    {
        if (!from)
            index = 0;
        if (body.chunked)
            parseChunkedBody(buffer);
        else if (body.content_length)
            Files::writeFile(fd, buffer + index, valread - index);
    }
    max_body_size -= valread - index;
}


void HttpRequest::parseChunkedBody(char *buffer)
{
    while (index < valread)
    {
        switch (flagMemory)
        {
            case 0: 
                if (buffer[index] == '\r')
                {
                    hex[last] = '\0';
                    size_dec = _stoi(hex, 16);
                    if (size_dec == 0) last_chunk = true;
                    hex[0] = '\0';
                    last = 0;
                    flagMemory = 1;
                    break;
                }
                else
                    hex[last++] = buffer[index];
                index++;
                if (last == 1024)
                    return(IsError(400));
                break;
            
            case 1:
            case 4:
                if (buffer[index] == '\r')
                {
                    flagMemory++;
                    index++;
                }
                else
                    return(IsError(400));
                break;

            case 2:
            case 5:
                if (buffer[index] == '\n')
                {
                    flagMemory++;
                    flagMemory %= 6;
                    index++;
                    if (last_chunk)
                        salat = true;
                }
                else
                    return(IsError(400));
                break;

            case 3:
                if (size_dec <= 0)
                {
                    flagMemory = 4;
                    break;
                }
                
                if (size_dec > valread - index)
                {
                    Files::writeFile(fd, buffer + index, valread - index);
                    size_dec -= valread - index;
                    index = valread;
                }
                else
                {
                    Files::writeFile(fd, buffer + index, size_dec);
                    index += size_dec;
                    flagMemory = 4;
                }
                break;
        }
    }
}

// RESPONSE
int HttpRequest::handleRequest()
{
    if (!is_error)
        return (0);
    return (error);
}

// *************************************************************************************** //
// ***                                Helper Functions                                 *** //
// *************************************************************************************** //
// GETTERS
int HttpRequest::getMethod() const
{
    return requestLine.method;
}

std::string HttpRequest::getPath() const
{
    return path;
}

std::string HttpRequest::getQuery() const
{
    return query;
}

std::string HttpRequest::getURI() const
{
    return res_uri;
}

std::string HttpRequest::getContentType()
{
    if (headers.find("Content-Type") == headers.end())
        return "";
    return headers["Content-Type"];
}

long int HttpRequest::getContentLenght()
{
    if (body.content_length)
        return _stoi(headers["Content-Length"], 10);
    return 0;
}

size_t HttpRequest::getContentLenght_CGI()
{
    if (requestLine.method != POST)
        return 0;
    if (body.chunked)
        return chunks_size;
    return _stoi(headers["Content-Length"], 10);
}

std::map<std::string, std::string> HttpRequest::getCookies()
{
    return cookie_args;
}

bool HttpRequest::getCookie()
{
    return cookie;
}

size_t HttpRequest::getPos() const
{
    return pos;
}

Location HttpRequest::getLocation() const
{
    return my_location;
}

Server HttpRequest::getServer() const
{
    return my_server;
}

std::string HttpRequest::getFileName()
{
    return body.file_name;
}

std::string HttpRequest::getExtension()
{
    std::string extension;

    if (path.find(".") != std::string::npos)
        extension = path.substr(path.find_last_of("."));
    return(extension);
}

int HttpRequest::getErrorCode() const
{
    return error;
}

int HttpRequest::getr_end() const
{
    return r_end;
}

// SETTERS
void HttpRequest::setConfig(ConfigFile &config)
{
    size_t queryPos;
    servers = config.getServers();
    FoundPath();
    if (location_found)
    {
        upload_path = my_location.upload;
        if (!upload_path.empty())
        {
            if (Files::isDir(upload_path) == -1)
                return(IsError(404));
            if (Files::isDir(upload_path) == 0)
                return(IsError(403));
        }
    }
    max_body_size = my_server.max_body_size;
    if (max_body_size < 0)
        return(IsError(413));
    queryPos = requestLine.uri.find('?');
    if (queryPos != std::string::npos)
    {
        query = requestLine.uri.substr(queryPos + 1);
        requestLine.uri = requestLine.uri.substr(0, queryPos);
        queryPos = path.find('?');
        path = path.substr(0, queryPos);
    }
}

void HttpRequest::setFirst(bool first)
{
    _first = first;
}

void HttpRequest::setValRead(int val)
{
    valread = val;
}

void HttpRequest::setFd(int _fd)
{
    fd = _fd;
}

// HELPERS
void HttpRequest::FoundPath()
{
    std::vector<Server>::iterator st;
    std::vector<Location>::iterator lt;

    if (requestLine.uri.find("http://") != std::string::npos)
        requestLine.uri = requestLine.uri.substr(requestLine.uri.find("http://") + 7);
    for (st = servers.begin(); st != servers.end(); st++)
    {
        if (requestLine.uri.find(st->host + ":" + _to_string(st->port)) != std::string::npos)
            requestLine.uri = requestLine.uri.substr(requestLine.uri.find(st->host + ":" + _to_string(st->port)) + st->host.length() + _to_string(st->port).length() + 1);
        for (lt = st->locations.begin(); lt != st->locations.end(); lt++)
        {
            if (requestLine.uri.find(lt->uri) == 0)
            {
                if (requestLine.uri == lt->uri)
                    path = lt->root;
                requestLine.uri = requestLine.uri.substr(lt->uri.length());
                my_server = *st;
                my_location = *lt;
                if (requestLine.uri != "")
                    path = lt->root + "/" + requestLine.uri;
                location_found = true;
                return;
            }
        }
        if (!location_found)
        {
            my_server = *st;
            if (requestLine.uri == "")
                path = st->root;
            else
                path = st->root + requestLine.uri;
            location_found = false;
            return;
        }
    }
}

bool HttpRequest::isLocationFound()
{
    return location_found;
}

void HttpRequest::IsError(int number)
{
    is_error = true;
    error = number;
}

bool HttpRequest::is_Error() const
{
    return is_error;
}

bool HttpRequest::isConnection() const
{
    return connection;
}

bool HttpRequest::enoughtData(long int global)
{
    if (body.chunked && salat)
            return true;
    if (body.content_length && (global >= getContentLenght()))
            return true;
    return false;
}

bool HttpRequest::enoughHeaders(char *buffer)
{
    for (int i = 0; i < valread; i++)
    {
        if (buffer[i] == '\r' && buffer[i + 1] == '\n' && buffer[i + 2] == '\r' && buffer[i + 3] == '\n')
        {
            pos = i;
            return true;
        }
    }
    return false;
}

void HttpRequest::createFile()
{
    std::string name;
    for (int i = 1; i < 1000; i++)
    {
        name = "./file" + _to_string(i);
        std::ifstream checkFile(name.c_str());
        if (!checkFile.good())
        {
            body.file_name = name;
            checkFile.close();
            break;
        }
        checkFile.close();
    }
}

size_t HttpRequest::_stoi(std::string str, int base)
{
    size_t number = 0;
    for (size_t i = 0; i < str.length(); i++)
    {
        if (str[i] >= '0' && str[i] <= '9')
            number = number * base + (str[i] - '0');
        else if (str[i] >= 'A' && str[i] <= 'F')
            number = number * base + (str[i] - 'A' + 10);
        else if (str[i] >= 'a' && str[i] <= 'f')
            number = number * base + (str[i] - 'a' + 10);
        else
            return -1;
    }
    return number;
}

std::string HttpRequest::_to_string(int num)
{
    std::string str;
    std::stringstream ss;
    ss << num;
    ss >> str;
    return str;
}

// *************************************************************************************** //