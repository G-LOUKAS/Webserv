#include "AutoIndex.hpp"

AutoIndex::AutoIndex() {
}

AutoIndex::~AutoIndex() {
}

void AutoIndex::setIndex(std::string dir_path, std::string autoindex_path, std::string uri)
{
    std::ofstream autoindex_file(autoindex_path.c_str());
    if (!autoindex_file)
        throw AutoIndexException();
    autoindex_file << "<html>\n";
    autoindex_file << "<head>\n";
    autoindex_file << "<title>TABLE OF CONTENTS</title>\n";
    autoindex_file << "<style>\n";
    autoindex_file << "body {\n";
    autoindex_file << "font-family: Arial, sans-serif;\n";
    autoindex_file << "margin: 0;\n";
    autoindex_file << "padding: 0;\n";
    autoindex_file << "}\n";
    autoindex_file << "div {\n";
    autoindex_file << "margin: 0 auto;\n";
    autoindex_file << "max-width: 800px;\n";
    autoindex_file << "padding: 20px;\n";
    autoindex_file << "}\n";
    autoindex_file << "h1 {\n";
    autoindex_file << "text-align: center;\n";
    autoindex_file << "margin-top: 0;\n";
    autoindex_file << "text-decoration: underline;\n";
    autoindex_file << "background-color: #56586e;\n";
    autoindex_file << "}\n";
    autoindex_file << "table {\n";
    autoindex_file << "width: 100%;\n";
    autoindex_file << "border-collapse: collapse;\n";
    autoindex_file << "background-color: #f2f2f2;\n";
    autoindex_file << "}\n";
    autoindex_file << "th, td {\n";
    autoindex_file << "padding: 10px;\n";
    autoindex_file << "border-bottom: 1px solid #ddd;\n";
    autoindex_file << "}\n";
    autoindex_file << "th {\n";
    autoindex_file << "background-color: #56586e;\n";
    autoindex_file << "}\n";
    autoindex_file << "</style>\n";
    autoindex_file << "</head>\n";
    autoindex_file << "<body>\n";
    autoindex_file << "<div>\n";
    autoindex_file << "<h1>Index of " + dir_path + "</h1>\n";
    autoindex_file << "<table>\n";
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(dir_path.c_str())) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (uri[uri.length() - 1] != '/')
                uri += '/';
            std::string name = ent->d_name;
            std::string gl_name = dir_path + '/' + name;
            autoindex_file << "<tr>\n";
            autoindex_file << "<td><a href=\"" + uri + name + "\">" + name + "</a></td>\n";
            autoindex_file << "<td>" + getLastTimeModified(gl_name) + "</td>\n";
            autoindex_file << "<td>" + getSize(gl_name) + "</td>\n";
            autoindex_file << "</tr>\n";
        }
        closedir(dir);
    }
    else
        throw AutoIndexException();
    autoindex_file << "</tbody>\n";
    autoindex_file << "</table>\n";
    autoindex_file << "</body>\n";
    autoindex_file << "</html>\n";
    autoindex_file.close();
}

std::string AutoIndex::getLastTimeModified(std::string path)
{
    struct stat file_stat;
    if (stat(path.c_str(), &file_stat) == 0)
    {
        struct tm *time = gmtime(&file_stat.st_mtime);
        char buffer[80];
        strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", time);
        return buffer;
    }
    return "";
}

std::string AutoIndex::getSize(std::string path)
{
    struct stat file_stat;
    if (stat(path.c_str(), &file_stat) == 0)
    {
        std::ostringstream oss;
        oss << file_stat.st_size;
        return oss.str();
    }
    return "";
}

const char *AutoIndex::AutoIndexException::what() const throw()
{
    return "AutoIndexException: Error opening file";
}