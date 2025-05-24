#include "HttpResponse.hpp"

// *************************************************************************************** //
// ***                            Constructor & Destructor                             *** //
// *************************************************************************************** //
HttpResponse::HttpResponse()
{
    statusCode = 0;
    statusMessage = "";
    res_body = "";
    auto_file = "";
    requestError = false;
    keep_alive = true;
    end = false;
    location_found = false;
    is_allowed = false;
    cgi_data.query = "";
    global_read = 0;
    fileOffset = 0;
    fileOpen = false;
    here = false;
    valread = 0;
    get_file = "";
    isZERO = false;
    position_p = 0;
    er_content = "";
    error_int = 1;
    error_index = 0;
    one_time = true;
    cgiend = true;
    firstcgi = true;
    setHeadersList();
}

HttpResponse::~HttpResponse()
{
}

// *************************************************************************************** //
// ***                                 HTTP Methods                                   *** //
// *************************************************************************************** //
// GET
std::string HttpResponse::ContentLenght(std::string content)
{
    std::ostringstream oss;

    oss << content.length();
    return oss.str();
}

void HttpResponse::GET(CGI &o_cgi)
{
    if (redirect != "")
    {
        statusCode = 301;
        headers["Location"] = redirect;
        return;
    }
    DIR *dir = opendir(path.c_str());
    if (dir)
    {
        closedir(dir);
        std::vector<std::string>::iterator it;
        bool foundIndex = false;
        for (it = index_args.begin(); it != index_args.end(); it++)
        {
            std::string indexPath = path + "/" + *it;
            if (access(indexPath.c_str(), F_OK) == 0)
            {
                if (*it == "index.php" || *it == "index.py")
                {
                    if (cgi)
                    {
                        cgi_data.path += '/' + *it;
                        cgi_data.file_extension = getExtension(*it);
                        if (firstcgi)
                            o_cgi.setAttributes(cgi_data, my_location.cgi);
                        if (o_cgi.yes_or_no(my_location.cgi))
                        {
                            if (firstcgi)
                                o_cgi.waitCgi();
                            if (o_cgi.getCgiEnd())
                            {
                                firstcgi = false;
                                cgiend = true;
                                if (o_cgi.getStatusCode() != 200)
                                {
                                    statusCode = o_cgi.getStatusCode();
                                    newcreated = false;
                                    return;
                                }
                                std::ifstream ofile(o_cgi.getCgiOutput().c_str(), std::ios::binary);
                                std::string line;
                                size_t lenght = 0;
                                while (std::getline(ofile, line))
                                {
                                    lenght = line.length() + 1;
                                    if (line == "" || line == "\r")
                                        break;
                                    std::string key = line.substr(0, line.find(":"));
                                    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                                    if (std::find(headers_list.begin(), headers_list.end(), key) == headers_list.end())
                                        break;
                                    std::string value = line.substr(line.find(":") + 1);
                                    position_p += lenght;
                                    headersCgi.insert(std::pair<std::string, std::string>(key, value));
                                }
                                get_file = o_cgi.getCgiOutput();
                                if (headersCgi.find("content-type") == headersCgi.end())
                                    headers["Content-Type"] = ext.get_content_type(get_file);
                                if (headersCgi.find("content-length") == headersCgi.end())
                                {
                                    std::ifstream file(get_file.c_str());
                                    file.seekg(0, std::ios::end);
                                    headers["Content-Length"] = _to_string((int)file.tellg() - position_p);
                                }
                                ofile.close();
                                statusCode = 200;
                                newcreated = true;
                                return;
                            }
                            else
                            {
                                cgiend = false;
                                newcreated = true;
                                return;
                            }
                        }
                    }
                }
                foundIndex = true;
                std::ifstream index_file(indexPath.c_str(), std::ios::binary);
                if (index_file)
                {
                    headers["Content-Type"] = ext.get_content_type(*it);
                    std::ifstream file(indexPath.c_str());
                    file.seekg(0, std::ios::end);
                    headers["Content-Length"] = _to_string(file.tellg());
                    file.close();
                    get_file = indexPath;
                    statusCode = 200;
                }
                else
                    statusCode = 403;
                break;
            }
        }
        if (!foundIndex)
        {
            if (autoindex)
            {
                AutoIndex o_autoIndex;
                std::string root_server = my_server.root;
                std::string autoindex_path = root_server + "/var/autoindex" + _to_string(fd_sock) + ".html";
                auto_file = autoindex_path;
                o_autoIndex.setIndex(path, autoindex_path, uri);
                std::ifstream ofile(autoindex_path.c_str(), std::ios::binary);
                headers["Content-Type"] = "text/html";
                ofile.seekg(0, std::ios::end);
                headers["Content-Length"] = _to_string(ofile.tellg());
                ofile.close();
                get_file = autoindex_path;
                statusCode = 200;
            }
            else
                statusCode = 403;
        }
    }
    else
    {
        std::ifstream file(path.c_str(), std::ios::binary);
        if (file)
        {
            if(cgi)
            {
                if (firstcgi)
                    o_cgi.setAttributes(cgi_data, my_location.cgi);
                if (o_cgi.yes_or_no(my_location.cgi))
                {
                    file.close();
                    if (firstcgi)
                        o_cgi.waitCgi();
                    if (o_cgi.getCgiEnd())
                    {
                        firstcgi = false;
                        cgiend = true;
                        if (o_cgi.getStatusCode() != 200)
                        {
                            statusCode = o_cgi.getStatusCode();
                            newcreated = false;
                            return;
                        }
                        std::ifstream ofile(o_cgi.getCgiOutput().c_str(), std::ios::binary);
                        std::string line;
                        position_p = 0;
                        size_t lenght = 0;
                        while (std::getline(ofile, line))
                        {
                            lenght = line.length() + 1;
                            if (line == "" || line == "\r")
                                break;
                            std::string key = line.substr(0, line.find(":"));
                            std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                            if (std::find(headers_list.begin(), headers_list.end(), key) == headers_list.end())
                                break;
                            std::string value = line.substr(line.find(":") + 1);
                            position_p += lenght;
                            headersCgi.insert(std::pair<std::string, std::string>(key, value));
                        }
                        get_file = o_cgi.getCgiOutput();
                        if (headersCgi.find("content-type") == headersCgi.end())
                            headers["Content-Type"] = ext.get_content_type(get_file);
                        if (headersCgi.find("content-length") == headersCgi.end())
                        {
                            std::ifstream file(get_file.c_str());
                            file.seekg(0, std::ios::end);
                            headers["Content-Length"] = _to_string((int)file.tellg() - position_p);
                        }
                        ofile.close();
                        statusCode = 200;
                        newcreated = true;
                        return;
                    }
                    else
                    {
                        cgiend = false;
                        newcreated = true;
                        return;
                    }
                }
            }
            headers["Content-Type"] = ext.get_content_type(path);
            std::ifstream file(path.c_str());
            file.seekg(0, std::ios::end);
            headers["Content-Length"] = _to_string(file.tellg());
            file.close();
            get_file = path;
            statusCode = 200;
        }
        else
            statusCode = 404;
    }
}

// POST
void HttpResponse::POST(CGI &o_cgi)
{
    if (cgi)
    {
        if (firstcgi)
            o_cgi.setAttributes(cgi_data, my_location.cgi);
        size_t lenght = 0;
        std::string line;
        if (o_cgi.yes_or_no(my_location.cgi))
        {
            if (firstcgi)
                o_cgi.waitCgi();
            if (o_cgi.getCgiEnd())
            {
                firstcgi = false;
                cgiend = true;
                Files::removeFile(o_cgi.getOutputname());
                if (o_cgi.getStatusCode() != 200)
                {
                    statusCode = o_cgi.getStatusCode();
                    newcreated = false;
                    return;
                }
                std::ifstream ofile(o_cgi.getCgiOutput().c_str());
                position_p = 0;
                while (std::getline(ofile, line))
                {
                    lenght = line.length() + 1;
                    if (line == "" || line == "\r")
                        break;
                    std::string key = line.substr(0, line.find(":"));
                    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
                    if (std::find(headers_list.begin(), headers_list.end(), key) == headers_list.end())
                        break;
                    std::string value = line.substr(line.find(":") + 1);
                    position_p += lenght;
                    headersCgi.insert(std::pair<std::string, std::string>(key, value));
                }
                get_file = o_cgi.getCgiOutput();;
                if (headersCgi.find("content-type") == headersCgi.end())
                    headers["Content-Type"] = ext.get_content_type(get_file);
                if (headersCgi.find("content-length") == headersCgi.end())
                {
                    std::ifstream file(get_file.c_str());
                    file.seekg(0, std::ios::end);
                    headers["Content-Length"] = _to_string((int)file.tellg() - position_p);
                }
                statusCode = 200;
                ofile.close();
                newcreated = true;
            }
            else
            {
                cgiend = false;
                newcreated = true;
                return;
            }
        }
    }
    statusCode = 201;
}

// DELETE
bool HttpResponse::isDirEmpty(std::string _path)
{
    int n = 0;
    struct dirent *d;
    DIR *dir = opendir(_path.c_str());
    if (dir == NULL)
        return true;
    while ((d = readdir(dir)) != NULL)
    {
        if (++n > 2)
            break;
    }
    closedir(dir);
    return (n <= 2);
}

void HttpResponse::DELETE(std::string _path)
{
    DIR *dir = opendir(_path.c_str());
    if (!dir)
    {
        closedir(dir);
        std::ifstream checkFile(_path.c_str());
        if (checkFile)
        {
            checkFile.close();
            if (std::remove(_path.c_str()) == 0)
            {
                statusCode = 204;
                return;
            }
        }
        else
            statusCode = 404;
    }
    else
    {
        if (!isDirEmpty(_path))
        {
            struct dirent *d;
            while ((d = readdir(dir)) != NULL)
            {
                std::string file_path = _path + "/" + d->d_name;
                if (d->d_type == DT_DIR)
                {
                    if (std::string(d->d_name) != "." && std::string(d->d_name) != "..")
                        DELETE(file_path);
                }
                else if (d->d_type == DT_REG)
                    DELETE(file_path);
            }
        }
        if (isDirEmpty(_path))
        {
            closedir(dir);
            if (rmdir(_path.c_str()) != 0)
                    statusCode = 403;
            else
                statusCode = 204;
        }
        else
        {
            closedir(dir);
            statusCode = 403;
        }
    }
}

// *************************************************************************************** //
// ***                               Public Functions                                  *** //
// *************************************************************************************** //
bool HttpResponse::createResponse(bool first)
{
    Codes o_codes;
    std::map<std::string, std::string>::iterator it;
    bool succ_get;

    if (!cgiend)
        return false;
    succ_get = ((statusCode == 200 || statusCode == 201) && (method == 0 || newcreated));
    if (first)
    {
        statusMessage = o_codes.getStatusMessage(statusCode);
        response = "";
        if (!succ_get)
            error_int = openErrorPage();
        setHeaders();
        if (headers.find("status") != headers.end())
            response.append("HTTP/1.1 " + headers["status"] + "\r\n");
        else 
            response.append("HTTP/1.1 " + _to_string(statusCode) + " " + statusMessage + "\r\n");
        for (it = headers.begin(); it != headers.end(); it++)
            response.append(it->first + ": " + it->second + "\r\n");
        response.append("\r\n");
        global_read = response.length();
        return false;
    }
    if (error_int == 0)
    {
        response.append(er_content);
        global_read = er_content.length();
        return true;
    }
    if (one_time)
    {
        if (newcreated)
        {
            if (Files::openIFile(fd_sock, get_file, position_p) == -1)
            {
                statusCode = 500;
                error_int = openErrorPage();
                one_time = false;
                return false;
            }
        }
        else 
        {
            if (Files::openIFile(fd_sock, get_file) == -1)
            {
                statusCode = 500;
                error_int = openErrorPage();
                one_time = false;
                return false;
            }
        }
        one_time = false;
    }
    char buffer[1024] = {0};
    if (isZERO)
        fileOffset -= valread;
    valread = Files::readFile(fd_sock, buffer, 1024);
    if (valread == 0)
    {
        Files::closeIFile(fd_sock);
        fileOpen = false;
        return true;
    }
    response.append(buffer, valread);
    global_read = valread;
    fileOffset += valread;
    if (fileOffset >= _stoi(headers["Content-Length"], 10))
    {
        Files::closeIFile(fd_sock);
        fileOpen = false;
        return true;
    }
    return false;
}

void HttpResponse::responseProcess(CGI &o_cgi)
{
    if (requestError)
    {
        end = true;
        return;
    }
    if (!is_allowed)
    {
        statusCode = 405;
        end = true;
        return;
    }
    if (method == 0)
        GET(o_cgi);
    else if (method == 1)
        POST(o_cgi);
    else if (method == 2)
        DELETE(path);
    end = true;
}

int HttpResponse::openErrorPage()
{
    if (error_args.find(statusCode) != error_args.end())
    {
        std::string error_file = error_args[statusCode];
        std::ifstream ofile(error_file.c_str(), std::ios::binary);
        if (ofile)
        {
            headers["Content-Type"] = "text/html";
            ofile.seekg(0, std::ios::end);
            headers["Content-Length"] = _to_string(ofile.tellg());
            get_file = error_file;
            ofile.close();
            return 1;
        }
    }
    std::string server_root = my_server.root;
    std::string error_file = server_root + "/Errors/html/" + _to_string(statusCode) + ".html";
    std::ifstream ofile(error_file.c_str(), std::ios::binary);
    if (ofile)
    {
        headers["Content-Type"] = "text/html";
        ofile.seekg(0, std::ios::end);
        headers["Content-Length"] = _to_string(ofile.tellg());
        get_file = error_file;
        ofile.close();
        return 1;
    }
    else
    {
        Codes o_codes;
        headers["Content-Type"] = "text/html";
        er_content = "<html>\n";
        er_content += "<head>\n";
        er_content += "<title>ERROR PAGE</title>\n";
        er_content += "<style>\n";
        er_content += "body {\n";
        er_content += "margin: 0;\n";
        er_content += "padding: 0;\n";
        er_content += "background-color: black;\n";
        er_content += "color: white;\n";
        er_content += "justify-content: center;\n";
        er_content += "}\n";
        er_content += "</style>\n";
        er_content += "</head>\n";
        er_content += "<body>\n";
        er_content += "<h1> ERROR" + _to_string(statusCode) + "</h1>\n";
        er_content += "<h2> ERROR" + o_codes.getStatusMessage(statusCode) + "</h2>\n";
        er_content += "</body>\n";
        er_content += "</html>\n";
        headers["Content-Length"] = ContentLenght(er_content);
        return 0;
    }
}

// *************************************************************************************** //
// ***                               Helper Functions                                  *** //
// *************************************************************************************** //
// GETTERS
std::string HttpResponse::getResponse()
{
    return response;
}

long int HttpResponse::getGlobalRead()
{
    return global_read;
}

std::string HttpResponse::getExtension(std::string path)
{
    std::string extension;

    if (path.find(".") != std::string::npos)
        extension = path.substr(path.find_last_of("."));
    return(extension);
}

bool HttpResponse::getCheckCgi()
{
    return cgiend;
}

std::string HttpResponse::getauto_file()
{
    return (auto_file);
}

// SETTERS
void HttpResponse::setConfigInfos()
{
    if (location_found)
    {
        autoindex = my_location.autoindex;
        index_args = my_location.index;
        redirect = my_location.redirect;
        if (my_location.cgi.size() > 0)
            cgi = true;
        isallowedMethod();
    }
    else
    {
        is_allowed = true;
        autoindex = my_server.autoindex;
        index_args = my_server.index;
    }
    error_args = my_server.errorPage;
    max_body_size = my_server.max_body_size;
}

void HttpResponse::setRequest(HttpRequest _request, int _fd)
{
    fd_sock = _fd;
    method = _request.getMethod();
    uri = _request.getURI();
    keep_alive = _request.isConnection();
    cookie = _request.getCookie();
    if (cookie)
        cookie_args = _request.getCookies();
    statusCode = _request.handleRequest();
    location_found = _request.isLocationFound();
    my_server = _request.getServer();
    if (location_found)
        my_location = _request.getLocation();
    path = _request.getPath();
    requestError = _request.is_Error();
    if (requestError)
    {
        statusCode = _request.getErrorCode();
        return;
    }
    cgi_data.method = method;
    cgi_data.fd_sock = fd_sock;
    cgi_data.path = path;
    cgi_data.query = _request.getQuery();
    cgi_data.file_name = _request.getFileName();
    cgi_data.file_extension = _request.getExtension();
    cgi_data.cookie = _request.getCookie();
    if (cookie)
        cgi_data.cookie_args = _request.getCookies();
    if (method == 1)
    {
        cgi_data.content_type = _request.getContentType();
        cgi_data.content_length = _request.getContentLenght_CGI();
    }
}

std::string HttpResponse::setDate()
{
    time_t now = time(0);
    struct tm *gmt = gmtime(&now);
    char buf[100];
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", gmt);

    return buf;
}

void HttpResponse::setHeaders()
{
    std::multimap<std::string, std::string>::iterator it;
    for (it = headersCgi.begin(); it != headersCgi.end(); it++)
        headers[it->first] = it->second;
    headers["Date"] = setDate();
    if (keep_alive)
        headers["Connection"] = "keep-alive";
    else
        headers["Connection"] = "close";
    if (cookie)
    {
        std::map<std::string, std::string>::iterator it;
        for (it = cookie_args.begin(); it != cookie_args.end(); it++)
            headers["Set-Cookie"] = it->first + "=" + it->second;
    }
}

void HttpResponse::setIsZero(bool _isZERO)
{
    isZERO = _isZERO;
}

void HttpResponse::setHeadersList()
{
    headers_list.push_back("accept-ch");
    headers_list.push_back("access-control-allow-origin");
    headers_list.push_back("accept-patch");
    headers_list.push_back("accept-ranges");
    headers_list.push_back("age");
    headers_list.push_back("allow");
    headers_list.push_back("alt-svc");
    headers_list.push_back("cache-control");
    headers_list.push_back("connection");
    headers_list.push_back("content-disposition");
    headers_list.push_back("content-encoding");
    headers_list.push_back("content-language");
    headers_list.push_back("content-length");
    headers_list.push_back("content-location");
    headers_list.push_back("content-md5");
    headers_list.push_back("content-range");
    headers_list.push_back("content-type");
    headers_list.push_back("date");
    headers_list.push_back("delta-base");
    headers_list.push_back("etag");
    headers_list.push_back("expires");
    headers_list.push_back("im");
    headers_list.push_back("last-modified");
    headers_list.push_back("link");
    headers_list.push_back("location");
    headers_list.push_back("p3p");
    headers_list.push_back("pragma");
    headers_list.push_back("preference-applied");
    headers_list.push_back("proxy-authenticate");
    headers_list.push_back("public-key-pins");
    headers_list.push_back("retry-after");
    headers_list.push_back("server");
    headers_list.push_back("set-cookie");
    headers_list.push_back("strict-transport-security");
    headers_list.push_back("trailer");
    headers_list.push_back("transfer-encoding");
    headers_list.push_back("tk");
    headers_list.push_back("upgrade");
    headers_list.push_back("vary");
    headers_list.push_back("via");
    headers_list.push_back("warning");
    headers_list.push_back("www-authenticate");
    headers_list.push_back("x-frame-options");
    headers_list.push_back("x-webkit-csp");
    headers_list.push_back("expect-ct");
    headers_list.push_back("nel");
    headers_list.push_back("permissions-policy");
    headers_list.push_back("refresh");
    headers_list.push_back("report-to");
    headers_list.push_back("status");
    headers_list.push_back("timing-allow-origin");
    headers_list.push_back("x-content-duration");
    headers_list.push_back("x-content-type-options");
    headers_list.push_back("x-powered-by");
    headers_list.push_back("x-redirect-by");
    headers_list.push_back("x-request-id");
    headers_list.push_back("x-ua-compatible");
    headers_list.push_back("x-xss-protection");
}

void HttpResponse::setResponse()
{
    response.clear();
}

// HELPERS
bool HttpResponse::keepAlive()
{
    return keep_alive;
}

bool HttpResponse::isEnd()
{
    return end;
}

void HttpResponse::isallowedMethod()
{
    if (my_location.allowMethods.size() == 0)
        is_allowed = true;
    else
    {
        std::vector<std::string>::iterator it;
        for (it = my_location.allowMethods.begin(); it != my_location.allowMethods.end(); it++)
        {
            if (*it == "GET" && method == 0)
            {
                is_allowed = true;
                break;
            }
            else if (*it == "POST" && method == 1)
            {
                is_allowed = true;
                break;
            }
            else if (*it == "DELETE" && method == 2)
            {
                is_allowed = true;
                break;
            }
        }
    }
}

size_t HttpResponse::_stoi(std::string str, size_t base)
{
    size_t number = 0;
    for (size_t i = 0; i < str.length(); i++)
        number = number * base + (size_t)(str[i] - '0');
    return number;
}

std::string HttpResponse::_to_string(long num)
{
    std::string str;
    std::stringstream ss;
    ss << num;
    ss >> str;
    return str;
}

// *************************************************************************************** //