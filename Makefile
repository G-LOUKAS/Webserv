NAME = webserv

CC = c++  -Wall -Wextra -Werror -g3 -std=c++98 #-fsanitize=address -g3

SRCS = Main/main.cpp Others/config_file/ConfigFile.cpp Others/web_server/WebServer.cpp Others/client/Client.cpp \
		Others/request/HttpRequest.cpp Others/response/HttpResponse.cpp Others/files/Files.cpp \
		Others/extentions/Ext.cpp Others/cgi/CGI.cpp Others/auto_index/AutoIndex.cpp Others/codes/Codes.cpp
OBJS = $(SRCS:.cpp=.o)
HEADERS = Others/config_file/ConfigFile.hpp Others/web_server/WebServer.hpp Others/client/Client.hpp \
		Others/request/HttpRequest.hpp Others/response/HttpResponse.hpp Others/files/Files.hpp \
		Others/extentions/Ext.hpp Others/cgi/CGI.hpp Others/auto_index/AutoIndex.hpp Others/codes/Codes.hpp
all: $(NAME)

$(NAME): $(SRCS) $(HEADERS)
	@$(CC) $(SRCS) -o $(NAME)
	@echo "\033[95m   _      _      _      _      _      _      _      _      _      _      _      _      _      _      _   \033[0m"
	@echo "\033[95m _( )_  _( )_  _( )_  _( )_  _( )_  _( )_  _( )_  _( )_  _( )_  _( )_  _( )_  _( )_  _( )_  _( )_  _( )_ \033[0m"
	@echo "\033[95m(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)\033[0m"
	@echo "\033[95m (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_) \033[0m"
	@echo "\033[95m   _                                                                                                 _   \033[0m"
	@echo "\033[95m _( )_     ____    __    ____  _______ .______        _______. _______ .______    ____    ____     _( )_ \033[0m"
	@echo "\033[95m(_ o _)    \   \  /  \  /   / |   ____||   _  \      /       ||   ____||   _  \   \   \  /   /    (_ o _)\033[0m"
	@echo "\033[95m (_,_)      \   \/    \/   /  |  |__   |  |_|  |    |   (----.|  |__   |  |_)  |   \   \/   /      (_,_) \033[0m"
	@echo "\033[95m   _         \            /   |   __|  |   _  <      \   \    |   __|  |      /     \      /         _   \033[0m"
	@echo "\033[95m _( )_        \    /\    /    |  |____ |  (_|  | .----)   |   |  |____ |  |\  \---.  \    /        _( )_ \033[0m"
	@echo "\033[95m(_ o _)        \__/  \__/     |_______||______/  |_______/    |_______|| _| \_____|   \__/        (_ o _)\033[0m"
	@echo "\033[95m (_,_)                                                                                             (_,_) \033[0m"
	@echo "\033[95m   _      _      _      _      _      _      _      _      _      _      _      _      _      _      _   \033[0m"
	@echo "\033[95m _( )_  _( )_  _( )_  _( )_  _( )_  _( )_  _( )_  _( )_  _( )_  _( )_  _( )_  _( )_  _( )_  _( )_  _( )_ \033[0m"
	@echo "\033[95m(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)(_ o _)\033[0m"
	@echo "\033[95m (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_)  (_,_) \033[0m"
	@echo ""

clean:
	@rm -f $(OBJS)

fclean: clean
	@rm -f $(NAME)

re: fclean all