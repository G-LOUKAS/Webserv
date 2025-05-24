#include "Ext.hpp"
#include "Ext.hpp"

// **************************************************** //
// ***           Constructor & Destructor           *** //
// **************************************************** //
Ext::Ext()
{
    extensions[".aac"] = "audio/aac";
    extensions[".abw"] = "application/x-abiword";
    extensions[".arc"] = "application/x-freearc";
    extensions[".avi"] = "video/x-msvideo";
    extensions[".azw"] = "application/vnd.amazon.ebook";
    extensions[".bin"] = "application/octet-stream";
    extensions[".bmp"] = "image/bmp";
    extensions[".bz"] = "application/x-bzip";
    extensions[".bz2"] = "application/x-bzip2";
    extensions[".csh"] = "application/x-csh";
    extensions[".css"] = "text/css";
    extensions[".csv"] = "text/csv";
    extensions[".doc"] = "application/msword";
    extensions[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
    extensions[".eot"] = "application/vnd.ms-fontobject";
    extensions[".epub"] = "application/epub+zip";
    extensions[".gz"] = "application/gzip";
    extensions[".gif"] = "image/gif";
    extensions[".htm"] = "text/html";
    extensions[".html"] = "text/html";
    extensions[".html"] = "text/html; charset=UTF-8";
    extensions[".ico"] = "image/vnd.microsoft.icon";
    extensions[".ics"] = "text/calendar";
    extensions[".jar"] = "application/java-archive";
    extensions[".jpeg"] = "image/jpeg";
    extensions[".jpg"] = "image/jpeg";
    extensions[".js"] = "text/javascript";
    extensions[".json"] = "application/json";
    extensions[".jsonld"] = "application/ld+json";
    extensions[".mid"] = "audio/midi audio/x-midi";
    extensions[".midi"] = "audio/midi audio/x-midi";
    extensions[".mjs"] = "text/javascript";
    extensions[".mp3"] = "audio/mpeg";
    extensions[".mp4"] = "video/mp4";
    extensions[".mpeg"] = "video/mpeg";
    extensions[".mpkg"] = "application/vnd.apple.installer+xml";
    extensions[".odp"] = "application/vnd.oasis.opendocument.presentation";
    extensions[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
    extensions[".odt"] = "application/vnd.oasis.opendocument.text";
    extensions[".oga"] = "audio/ogg";
    extensions[".ogv"] = "video/ogg";
    extensions[".ogx"] = "application/ogg";
    extensions[".opus"] = "audio/opus";
    extensions[".otf"] = "font/otf";
    extensions[".png"] = "image/png";
    extensions[".PNG"] = "image/png";
    extensions[".pdf"] = "application/pdf";
    extensions[".php"] = "application/x-httpd-php";
    extensions[".ppt"] = "application/vnd.ms-powerpoint";
    extensions[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
    extensions[".rar"] = "application/vnd.rar";
    extensions[".rtf"] = "application/rtf";
    extensions[".sh"] = "application/x-sh";
    extensions[".svg"] = "image/svg+xml";
    extensions[".swf"] = "application/x-shockwave-flash";
    extensions[".tar"] = "application/x-tar";
    extensions[".tif"] = "image/tiff";
    extensions[".tiff"] = "image/tiff";
    extensions[".ts"] = "video/mp2t";
    extensions[".ttf"] = "font/ttf";
    extensions[".txt"] = "text/plain";
    extensions[".vsd"] = "application/vnd.visio";
    extensions[".wav"] = "audio/wav";
    extensions[".weba"] = "audio/webm";
    extensions[".webm"] = "video/webm";
    extensions[".webp"] = "image/webp";
    extensions[".woff"] = "font/woff";
    extensions[".woff2"] = "font/woff2";
    extensions[".xhtml"] = "application/xhtml+xml";
    extensions[".xls"] = "application/vnd.ms-excel";
    extensions[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
    extensions[".xml"] = "application/xml";
    extensions[".xul"] = "application/vnd.mozilla.xul+xml";
    extensions[".zip"] = "application/zip";
    extensions[".3gp"] = "video/3gpp";
    extensions[".3g2"] = "video/3gpp2";
    extensions[".7z"] = "application/x-7z-compressed";
}

Ext::~Ext()
{
    extensions.clear();
}

// **************************************************** //
// ***           Public Member Functions            *** //
// **************************************************** //
std::string Ext::get_content_type(std::string file_name)
{
    std::string extension;
    if (file_name.find(".") != std::string::npos)
        extension = file_name.substr(file_name.find_last_of("."));
    if (extensions.find(extension) != extensions.end())
        return extensions[extension];
    return "text/html";
}

std::string Ext::get_extension(std::string content_type_value)
{
    for (std::map<std::string, std::string>::iterator it = extensions.begin(); it != extensions.end(); ++it)
    {
        if (it->second == content_type_value)
            return it->first;
    }
    return ".html";
}