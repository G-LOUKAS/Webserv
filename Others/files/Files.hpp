#ifndef HTTP_FILES_HPP
#define HTTP_FILES_HPP

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

class Files
{
private:
    // **************************************************** //
    // ***                 Data Members                 *** //
    // **************************************************** //
    static std::map<int, std::ofstream*> fileMap;
    static std::map<int, std::string> filesName;
    static std::map<int, std::ifstream*> ifileMap;
    // **************************************************** //
    // ***               CANONICAL FORM                 *** //
    // **************************************************** //
    Files();
    virtual ~Files();
    // **************************************************** //
public:
    // **************************************************** //
    // ***           Public Member Functions            *** //
    // **************************************************** //
    static int openFile(int id, std::string fileName);
    static int openIFile(int id, std::string fileName);
    static int openIFile(int id, std::string fileName, int position);
    static void writeFile(int id, char *data, int size);
    static int readFile(int id, char *data, int size);
    static void closeFile(int id);
    static void closeIFile(int id);
    static void removeFiles(int id);
    static void removeFile(std::string fileName);
    // static void closeFilee(std::string fileName);
    static int isDir(std::string path);
    static bool fileExists(std::string fileName);
    // **************************************************** //
};

#endif