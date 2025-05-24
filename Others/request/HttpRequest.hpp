#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <errno.h>
#include <map>
#include <vector>
#include <algorithm>
#include <list>
#include "../config_file/ConfigFile.hpp"
#include "../extentions/Ext.hpp"
#include "../files/Files.hpp"

typedef struct CGIData
{
    int method;
    std::string path;
    std::string query;
    int fd_sock;
    std::string file_name;
    std::string file_extension;
    std::string content_type;
    size_t content_length;
    bool cookie;
    std::map<std::string, std::string> cookie_args;
} CGIData;
class HttpRequest
{
private:
    // **************************************************** //
    // ***                   Typedefs                   *** //
    // **************************************************** //
    typedef enum ValidMethods
    {
        GET,
        POST,
        DELETE,
    } Methods;
    typedef struct RequestLine
    {
        ValidMethods method;
        std::string uri;
        std::string version;
    } RequestLine;
    typedef struct Body
    {
        bool chunked;
        bool content_length;
        std::string file_name;
        std::string file_extension;
    } Body;
    // **************************************************** //
    // ***                 Data Members                 *** //
    // **************************************************** //
    RequestLine requestLine;
    std::map<std::string, std::string> headers;
    Body body;
    std::string path;
    std::string query;
    std::vector<Server> servers;
    Server my_server;
    long long int max_body_size;
    bool location_found;
    std::string upload_path;
    std::string res_body;
    std::map<std::string, std::string> cookie_args;
    bool cookie;
    Location my_location;
    Ext ext;
    bool newcreated;
    bool is_error;
    int error;
    bool connection;
    bool _first;
    char tmp[10000];
    int index;
    int r_end;
    int valread;
    long int pos;
    int fd;
    std::string res_uri;
    int flagMemory;
    bool last_chunk;
    // **************************************************** //
    long long int chunks_size;   // the sum of all chunks
    
    char hex[1024];         // the size of the chunk in hex
    long int size_dec;      // the size of the chunk in decimal
    long int last;          // the last position of the last size not finished yet

    // long int last_chunk;    // the size needed to finish the last chunk
    
    bool salat;
    // **************************************************** //

public:
    // **************************************************** //
    // ***           Constructor & Destructor           *** //
    // **************************************************** //
    HttpRequest();
    ~HttpRequest();
    // **************************************************** //
    // ***                Member Functions              *** //
    // **************************************************** //
    // LINE
    void parseRequestLine(char *buffer, ConfigFile &config);
    void parseMethod(std::string _method);
    void parseTarget(std::string _uri);
    void parseVersion(std::string _version);
    // HEADERS
    void parseHeaders(char *buffer, bool headers_complete);
    void parseHeader(std::string header);
    // BODY
    void parseBody(char *buffer, bool from);
    void parseChunkedBody(char *buffer);
    // RESPONSE
    int handleRequest();
    // **************************************************** //
    // ***               Helper Functions               *** //
    // **************************************************** //
    // GETTERS
    int getMethod() const;
    std::string getPath() const;
    std::string getQuery() const;
    std::string getURI() const;
    std::string getContentType();
    long int getContentLenght();
    size_t getContentLenght_CGI();
    std::map<std::string, std::string> getCookies();
    bool getCookie();
    size_t getPos() const;
    Location getLocation() const;
    Server getServer() const;
    std::string getFileName();
    std::string getExtension();
    bool getNewCreatedCGI() const;
    int getErrorCode() const;
    int getr_end() const;
    // SETTERS
    void setConfig(ConfigFile &config);
    void setFirst(bool first);
    void setValRead(int val);
    void setFd(int _fd);
    // HELPERS
    void FoundPath();
    bool isLocationFound();
    void IsError(int number);
    bool is_Error() const;
    bool isConnection() const;
    bool enoughtData(long int global);
    bool enoughHeaders(char *buffer);
    void createFile(); // create the file for the body in the case of cgi
    size_t _stoi(std::string str, int base);
    std::string _to_string(int num);
    // **************************************************** //
};

#endif