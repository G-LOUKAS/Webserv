#ifndef AUTOINDEX_HPP
#define AUTOINDEX_HPP


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <exception>
#include <ctime>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>




class AutoIndex {
    public:
        AutoIndex();
        ~AutoIndex();
        void setIndex(std::string dir_path, std::string autoindex_path, std::string uri);
        std::string getLastTimeModified(std::string path);
        std::string getSize(std::string path);
        class AutoIndexException : public std::exception {
            public:
                virtual const char* what() const throw();
        };
};



#endif