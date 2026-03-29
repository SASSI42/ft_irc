NAME = ircserv

CC = c++
MAND = Server/Channel.cpp Server/Client.cpp main.cpp Server/Server.cpp Server/Exceptions.cpp Server/utils.cpp commands/Commands.cpp commands/InviteCommand.cpp commands/JoinCommand.cpp commands/KickCommand.cpp commands/ModeCommand.cpp commands/NickCommand.cpp commands/PassCommand.cpp commands/PrivmsgCommand.cpp commands/TopicCommand.cpp commands/UserCommand.cpp commands/CommandDispatcher.cpp
MAND_OBJS = $(MAND:%.cpp=%.o)
FLAGS = -std=c++98 -Wall -Werror -Wextra

all : $(NAME)

$(NAME) : $(MAND_OBJS) 
	$(CC) $(FLAGS) $^ -o $@

%.o : %.cpp Server/Channel.hpp Server/Client.hpp Server/replies.hpp Server/Server.hpp Server/utils.hpp Server/Exceptions.hpp commands/Commands.hpp commands/CommandDispatcher.hpp
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(MAND_OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all