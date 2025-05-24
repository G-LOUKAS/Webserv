#include "Files.hpp"

// *************************************************************************************** //
// ***                             Data Members                                      *** //
// *************************************************************************************** //
std::map<int, std::ofstream *> Files::fileMap;
std::map<int, std::string> Files::filesName;
std::map<int, std::ifstream *> Files::ifileMap;

// *************************************************************************************** //
// ***                             Constructor & Destructor                            *** //
// *************************************************************************************** //
Files::Files()
{
}

Files::~Files()
{
}

// *************************************************************************************** //
// ***                             Public Member Functions                             *** //
// *************************************************************************************** //
int Files::openFile(int id, std::string fileName)
{
    if (fileMap.find(id) != fileMap.end())
        return -1;
    std::ofstream *file = new std::ofstream(fileName.c_str());
    if (!file->is_open())
    {
        delete file;
        return -1;
    }
    fileMap[id] = file;
    filesName[id] = fileName;
    return 0;
}

int Files::openIFile(int id, std::string fileName)
{
    if (ifileMap.find(id) != ifileMap.end())
        return -1;
    std::ifstream *file = new std::ifstream(fileName.c_str() , std::ios::binary);
    if (!file->is_open())
    {
        delete file;
        return -1;
    }
    ifileMap[id] = file;
    return 0;
}

int Files::openIFile(int id, std::string fileName, int position)
{
    if (ifileMap.find(id) != ifileMap.end())
        return -1;
    std::ifstream *file = new std::ifstream(fileName.c_str(), std::ios::binary);
    if (!file->is_open())
    {
        delete file;
        return -1;
    }
    file->seekg(position);
    ifileMap[id] = file;
    return 0;
}

void Files::writeFile(int id, char *data, int size)
{
    if (fileMap.find(id) == fileMap.end())
        return;
    fileMap[id]->write(data, size);
    fileMap[id]->flush();
}

int Files::readFile(int id, char *data, int size)
{
    if (ifileMap.find(id) == ifileMap.end())
        return -1;
    ifileMap[id]->read(data, size);
    return ifileMap[id]->gcount();
}

void Files::closeFile(int id)
{
    if (fileMap.find(id) == fileMap.end())
        return;
    fileMap[id]->close();
    delete fileMap[id];
    fileMap.erase(id);
}

void Files::closeIFile(int id)
{
    if (ifileMap.find(id) == ifileMap.end())
        return;
    ifileMap[id]->close();
    delete ifileMap[id];
    ifileMap.erase(id);
}

void Files::removeFiles(int id)
{
    if (fileMap.find(id) == fileMap.end())
        return;
    fileMap[id]->close();
    remove(filesName[id].c_str());
    if (fileMap[id])
        delete fileMap[id];
    fileMap.erase(id);
}

void Files::removeFile(std::string fileName)
{
    remove(fileName.c_str());
}

int Files::isDir(std::string path)
{
    DIR *dir = opendir(path.c_str());
    if (dir)
    {
        closedir(dir);
        return 1;
    }
    else
    {
        closedir(dir);
        std::ifstream file(path.c_str());
        if (file.good())
        {
            file.close();
            return 0;
        }
    }
    return -1;
}

bool Files::fileExists(std::string fileName)
{
    bool exists = false;
    std::ofstream file(fileName.c_str());
    if (file.good())
    {
        exists = true;
        file.close();
    }
    file.close();
    return exists;
}
// *************************************************************************************** //