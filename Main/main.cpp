#include "../Others/web_server/WebServer.hpp"
#include "../Others/client/Client.hpp"
#include "../Others/config_file/ConfigFile.hpp"
#include <csignal>

int main(int argc, char **argv) 
{
    try {
            std::string file;
            if (argc < 2) 
                file = "Configuration/Default.conf";
            else
                file = argv[1];
            ConfigFile configFile(file);
            configFile.ParsConfigFile();
            std::vector<Server> servers = configFile.getServers();
            configFile.check_servers(servers);
            WebServer webServer(configFile);
            signal(SIGPIPE, SIG_IGN);
            webServer.setupServers();
            webServer.run();

    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}