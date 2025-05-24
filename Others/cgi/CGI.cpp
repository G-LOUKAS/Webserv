#include "CGI.hpp"

// **************************************************** //
// ***           Constructor & Destructor           *** //
// **************************************************** //

CGI::CGI()
{  
    statusCode = 200;
    startTime = time(NULL);
    cgiEnd = true;
    counter = 0;
}

CGI::~CGI()
{
}

// **************************************************** //
// ***              Public Functions                *** //
// **************************************************** //
bool CGI::yes_or_no(std::vector<std::string> args)
{
    for (std::vector<std::string>::iterator it = args.begin(); it != args.end(); ++it)
        if (*it == file_extension)
            return true;
    return false;
}

int CGI::getStatusCode()
{
    return statusCode;
}

std::string CGI::getMethode(int method)
{
    if (method == 0)
        return "GET";
    else if (method == 1)
        return "POST";
    else
        return "DELETE";
}

std::string CGI::getContentLength(size_t _contentLength)
{
    if (_contentLength == 0)
        return "";
    else
        return toString(_contentLength);
}

void CGI::setAttributes(CGIData _cgiData, std::vector<std::string> args)
{
    file_extension = _cgiData.file_extension;
    if (!yes_or_no(args))
        return;
    method = getMethode(_cgiData.method);
    queryString = _cgiData.query;
    ScriptPath = _cgiData.path;
    fd_sock = _cgiData.fd_sock; 
    file_name = _cgiData.file_name;
    contentType = _cgiData.content_type;
    contentLength = getContentLength(_cgiData.content_length);
    cookie = _cgiData.cookie;
    if (cookie)
        cookie_args = _cgiData.cookie_args;
    cmdPath = "";
    if (cgiEnd)
        executeScript();
}

char** CGI::setEnvironment()
{
    std::vector<std::string> envVars;
    envVars.push_back("REQUEST_METHOD=" + method);
    if (method == "POST") 
    {
        if (!contentType.empty())
            envVars.push_back("CONTENT_TYPE=" + contentType);
        if (!contentLength.empty())
            envVars.push_back("CONTENT_LENGTH=" + contentLength);
    }
    else if (method == "GET") 
    {
        if (!queryString.empty())
            envVars.push_back("QUERY_STRING=" + queryString);
    }
    if (cookie)
    {
        std::string cookieStr = "";
        for (std::map<std::string, std::string>::iterator it = cookie_args.begin(); it != cookie_args.end(); ++it)
            cookieStr += it->first + "=" + it->second + ";";
        envVars.push_back("HTTP_COOKIE=" + cookieStr);
    }
    envVars.push_back("SCRIPT_FILENAME=" + ScriptPath);
    envVars.push_back("GATEWAY_INTERFACE=CGI/1.1");
    envVars.push_back("SERVER_PROTOCOL=HTTP/1.1");
    envVars.push_back("REDIRECT_STATUS=200");
    char** cEnv = new char*[envVars.size() + 1];
    for (size_t i = 0; i < envVars.size(); i++) 
        cEnv[i] = strdup(envVars[i].c_str());
    cEnv[envVars.size()] = NULL;
    return cEnv;
}

bool CGI::getCgiEnd()
{
    return cgiEnd;
}

std::string CGI::toString(size_t number)
{
    std::stringstream ss;
    ss << number;
    return ss.str();
}

std::string CGI::checkFileExtension(std::string ext)
{
    if (ext == ".py")
        return "/usr/bin/python3";
    else if (ext == ".php")
        return "/usr/bin/php-cgi";
    else
        return "";
}

void CGI::executeScript()
{ 
    std::string name = "./cgiOutput";
    int i = 0;
    while (true)
    {
        file = name + toString(i) + ".html";
        if (access(file.c_str(), F_OK) == -1)
            break;
        i++;
    }
    startTime = time(NULL);
    cgiEnd = false;
    pid = fork();
    if (pid == -1) 
    {
        std::cerr << "Error: Fork failed\n";
        statusCode = 500; 
        cgiEnd = true;
        return;
    }
    if (pid == 0)
    {
        char **envVar = setEnvironment();
        if (!std::freopen(file.c_str(), "w+", stdout))
        {
            std::cerr << "Error: Opening output file at " << file << "\n";
            std::fclose(stdout);
            statusCode = 500;
            cgiEnd = true;
            return;
        }
        cmdPath = checkFileExtension(file_extension);
        if (cmdPath.empty()) 
        {
            std::cerr << "Error: Unsupported file extension\n";
            exit(EXIT_FAILURE);
        }
        if (method == "POST")
        {
            if (!std::freopen(file_name.c_str(), "r", stdin))
            {
                std::cerr << "Error: Opening input file\n";
                std::fclose(stdin);
                exit(100);
            }
            Files::closeFile(fd_sock);
        }
        size_t found = ScriptPath.find_last_of('/');
        if (found != std::string::npos)
            chdir(ScriptPath.substr(0,found).c_str());
        char *args[3];
        args[0] = (char *)cmdPath.c_str();
        args[1] = (char*)ScriptPath.c_str();
        args[2] = NULL;
        if (execve(args[0], args, envVar) == -1)
        {
            perror("execve failed");
            exit(100);
        }
    }
}

void CGI::waitCgi()
{
    int status;
    int timeout = 10; 
    pid_t result = waitpid(pid, &status, WNOHANG);
  
    if (result == 0)
    {
        time_t endTime = time(NULL);
        if (endTime - startTime > timeout) 
        {
            result = 1;
            statusCode = 408;
            cgiEnd = true;
            if (kill(pid, SIGKILL) == 0)
                waitpid(pid, &status, 0);
            return;
        }
    }
    else if (result == -1) 
    {
        std::cerr << "Error: waitpid failed\n";
        statusCode = 500;
        cgiEnd = true;
    }
    else 
    {
        if (WIFEXITED(status))
        {
            int exitStatus = WEXITSTATUS(status);    
            if (exitStatus == 100)
            {
                statusCode = 502;
                cgiEnd = true;
            }
            cgiEnd = true;
        }
        else if (WIFSIGNALED(status))
        {
            statusCode = 500;
            cgiEnd = true;
        }
        else
            cgiEnd = true;
    }
}

std::string CGI::getCgiOutput()
{
    return (file);
}

std::string CGI::getOutputname()
{
    return (file_name);
}