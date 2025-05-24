#ifndef EXT_HPP
#define EXT_HPP

#include <unistd.h>
#include <iostream>
#include <string>
#include <map>

class Ext{
private:
    // **************************************************** //
    // ***                 Data Members                 *** //
    // **************************************************** //
    std::map<std::string, std::string> extensions;
    // **************************************************** //

public:
    // **************************************************** //
    // ***            Constructor & Destructor          *** //
    // **************************************************** //
    Ext();
    ~Ext();
    // **************************************************** //
    // ***           Public Member Functions            *** //
    // **************************************************** //
    std::string get_content_type(std::string file_name);
    std::string get_extension(std::string content_type_value);
    // **************************************************** //
};

#endif