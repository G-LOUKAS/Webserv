#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include <fcntl.h>
#include "../config_file/ConfigFile.hpp"
#include "../request/HttpRequest.hpp"
#include "../extentions/Ext.hpp"
#include "../auto_index/AutoIndex.hpp"
#include "../codes/Codes.hpp"
#include "../cgi/CGI.hpp"
#include "../files/Files.hpp"

class CGI;
class HttpResponse {
private:
    // ****************************************************** //
    // ***                 Data Members                   *** //
    // ****************************************************** //
    int method;
    std::string uri;
    std::string response;
    bool requestError;
    int statusCode;
    std::string body;
    std::string statusMessage;
    std::multimap<std::string, std::string> headersCgi;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> cookie_args;
    std::map<int, std::string> error_args;
    std::string redirect;
    std::string res_body;
    bool end;
    bool cgiend;
    bool firstcgi;
    bool keep_alive;
    bool cookie;
    bool one_time;
    Ext ext;
    long int global_read;
    bool newcreated;
    bool here;
    std::string get_file;
    size_t fileOffset;
    bool fileOpen;
    int valread;
    int fd_sock;
    bool isZERO;
    size_t position_p;
    std::list<std::string> headers_list;
    std::string er_content;
    int error_int;
    int error_index;
    std::string auto_file;
    // * FROM ConfigFile * //
    std::string path;
    Server my_server;
    bool location_found;
    Location my_location;
    bool autoindex;
    bool  cgi;
    unsigned int max_body_size;
    bool is_allowed;
    std::vector<std::string> index_args;
    std::string error_page;
    // ****************************************************** //

public:
    // ****************************************************** //
    // ***              Constructor & Destructor          *** //
    // ****************************************************** //
    HttpResponse();
    ~HttpResponse();
    // ****************************************************** //
    // ***                 HTTP Methods                   *** //
    // ****************************************************** //
    // GET
    std::string ContentLenght(std::string content);
    void GET(CGI &o_cgi);
    // POST
    void POST(CGI &o_cgi);
    // DELETE
    bool isDirEmpty(std::string _path);
    void DELETE(std::string _path);
    // ****************************************************** //
    // ***                 Public Functions               *** //
    // ****************************************************** //
    bool createResponse(bool first);
    void responseProcess(CGI &o_cgi);
    int openErrorPage();
    // ****************************************************** //
    // ***                 Helper Functions               *** //
    // ****************************************************** //
    // GETTERS
    std::string getResponse();
    long int getGlobalRead();
    std::string getExtension(std::string _path);
    bool getCheckCgi();
    std::string getauto_file();
    // SETTERS
    void setConfigInfos();
    void setRequest(HttpRequest request, int _fd);
    std::string setDate();
    void setHeaders();
    void setIsZero(bool _isZERO);
    void setHeadersList();
    void setResponse();
    // HELPERS
    bool keepAlive();
    bool isEnd();
    void isallowedMethod();
    size_t _stoi(std::string str, size_t base);
    std::string _to_string(long num);
    // * FOR CGI * //
    CGIData cgi_data;
    // ****************************************************** //
};

#endif
