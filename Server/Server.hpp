#ifndef SERVER_H
#define SERVER_H

#include <string>
#include <set>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdexcept>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <algorithm>
#include <unistd.h>
#include "Client.hpp"
#include "Channel.hpp"
#include "utils.hpp"
#include <iostream>
#include <cstdlib>
#include "../commands/Commands.hpp"
#include "../commands/CommandDispatcher.hpp"
class CommandDispatcher;

class Server{
	private :
		static int					serverSocket;
		static CommandDispatcher 	dispatcher;
		static std::vector<pollfd>	fds;
		static std::vector<Client>	clients;
		static std::vector<Channel>	channels;
		static std::string			password;
		static int					port;
		static time_t				rawtime;
		static struct tm* 			timeinfo;
		static bool					running;
	public :
		static struct tm*			geTimeInfo();
		static void 				broadcastToUsers(std::string& message);
		static void 				parse(int arc, char **arg);
		static std::string			getPassword();
		static std::vector<Client>	getClients();
		static Client*				getClient(std::string nickname);
		static std::string			getClientNicknameByFd(int fd);
		static Channel*				getChannel(std::string name);
		static void					addChannel(Channel& channel);
		static void					removeChannel(std::string& channelName);
		static void					removeClientFromChannels(Client& client, std::string flag);
		static void					init();
		static void					run();
		static void					acceptClient();
		static void					handleClientMsg(int fd);
		static void					eraseClient(Client &client);
		static void					shutdown(int signum);
		static void					cleanResources();
		static void 				closeServerSocket();
};
#endif