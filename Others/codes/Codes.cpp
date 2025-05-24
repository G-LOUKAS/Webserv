#include "Codes.hpp"

// ****************************************************** //
// ***              Constructor & Destructor          *** //
// ****************************************************** //
Codes::Codes()
{
    statusCodes[200] = "OK";
    statusCodes[201] = "Created";
    statusCodes[204] = "No Content";
    statusCodes[400] = "Bad Request";
    statusCodes[403] = "Forbidden";
    statusCodes[404] = "Not Found";
    statusCodes[405] = "Method Not Allowed";
    statusCodes[408] = "Request Timeout";
    statusCodes[413] = "Payload Too Large";
    statusCodes[502] = "Bad Gateway";
    statusCodes[505] = "HTTP Version Not Supported";
}

Codes::~Codes()
{
    statusCodes.clear();
}

// ****************************************************** //
// ***                 Public Functions               *** //
// ****************************************************** //
std::string Codes::getStatusMessage(int code)
{
    return statusCodes[code];
}
