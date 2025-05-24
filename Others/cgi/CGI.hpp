#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <cstdio>
#include <fstream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <fcntl.h>
#include <sys/wait.h>
#include <algorithm>
#include <map>
#include <vector>
#include "../response/HttpResponse.hpp"

class HttpResponse;
class CGI{
    private:
        /* *****FROM REQUEST***** */
        std::string method;
        std::string queryString;
        std::string ScriptPath;
        int fd_sock;
        std::string file_name;
        std::string file_extension;
        std::string contentType;
        std::string contentLength;
        bool cookie;
        std::map<std::string, std::string> cookie_args;
        /* ********************** */
        std::string cmdPath;
        std::string file;
        int statusCode;
        time_t startTime;
        bool cgiEnd;
        pid_t pid;
        int counter;
    public:
        // ****************************************************** //
        // ***              Constructor & Destructor          *** //
        // ****************************************************** //
        CGI();
        ~CGI();
        // ****************************************************** //
        // ***                 Public Functions               *** //
        // ****************************************************** //
        bool yes_or_no(std::vector<std::string> args);
        int getStatusCode();
        std::string getMethode(int method);
        std::string getContentLength(size_t _contentLenght);
        void setAttributes(CGIData _cgiData, std::vector<std::string> args);
        void setQueryMap();
        char **setEnvironment();
        std::string checkFileExtension(std::string ext);
        void executeScript();
        void waitCgi();
        std::string getCgiOutput();
        std::string getOutputname();
        void freeEnv(char **env);
        bool getCgiEnd();
        // to string
        std::string toString(size_t number);
};  

#endif
