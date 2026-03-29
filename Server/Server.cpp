#include "Server.hpp"

int						Server::serverSocket = -1; 
std::vector<Client>		Server::clients;
std::vector<pollfd>		Server::fds;
std::vector<Channel>	Server::channels;
std::string				Server::password;
int						Server::port;
time_t 					Server::rawtime;
struct tm* 				Server::timeinfo;
CommandDispatcher		Server::dispatcher; 
bool					Server::running = true;


void Server::closeServerSocket()
{
 	if (serverSocket != -1)
		close(serverSocket);
}

void Server::parse(int arc, char **arg)
{
	long		port;
	char		*delimiter;

	if ( arc != 3 )
		throw InitException("Usage: ./ircserv <port> <password>");
	std::string	password = arg[2];
	port = strtol(arg[1], &delimiter, 10);
	if ( isValidPort(port, delimiter) == false )
		throw InitException("Port should be in the [1024, 65535] range.");
	if ( isValidPassword(password) == false )
		throw InitException("Nice try, hacker. Passwords can't be empty, 500km long, or full of enter keys. Try again like a civilized human.\n");
	Server::password = password;
	Server::port = port;
}

void Server::init()
{
	int			yes = 1;
	sockaddr_in	serverAddr;

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if ( serverSocket < 0 )
		throw InitException("Failed to create a socket.");
	if ( fcntl(serverSocket, F_SETFL, O_NONBLOCK) == -1 )
		throw InitException("Failed to set NONBLOCK flag on file descriptor.");
	if ( setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1 )
		throw InitException("Failed to set SO_REUSEADDR on Server socket.");

	//Initialize server address with IPv4, port, and IP settings
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(Server::port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	if ( bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == -1 )
		throw InitException("Failed to bind socket to address.");
	if ( listen(serverSocket, SOMAXCONN) == -1 )
		throw InitException("Failed to listen on socket.");
	time(&rawtime);
	timeinfo = localtime(&rawtime);
}

void Server::run()
{
	int	nfds = 0;

	addToFds(fds, serverSocket);
	std::cout << "Server is listenning on port " << Server::port << " ...\n";
	while (running)
	{
		nfds = poll(&fds[0], fds.size(), -1);
		if ( nfds < 0 && errno != EINTR)
			throw RunException("poll() : Unable to monitor file descriptors.");
		else if ( nfds <= 0 )
			continue;

		for (unsigned int i = 0; i < fds.size(); i++)
		{
			if ( (fds[i].revents & POLLIN) == false )
				continue;
			if ( fds[i].fd == serverSocket )
				acceptClient();
			else
				handleClientMsg( fds[i].fd );
		}
	}
}

void Server::shutdown(int signum)
{
	(void)signum;
	running = false;
}

void Server::cleanResources()
{
	for (std::vector<pollfd>::iterator it = fds.begin(); it != fds.end(); it++)
	{
		if (it->fd >= 0)
		{
			close(it->fd);
			it->fd = -1;
		}
	}
}

void Server::acceptClient()
{
	int	clientSocket;
	struct sockaddr_in clientAddr;
	socklen_t len = sizeof(clientAddr);

	clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &len);
	if ( clientSocket < 0 )
		throw RunException("accept() : Unable to accept incoming client connection.");
	if ( fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1 )
	{
		close(clientSocket);
		throw RunException("Failed to set NONBLOCK flag on file descriptor.");
	}
	addToFds(fds, clientSocket);
	clients.push_back(Client(clientSocket, inet_ntoa(clientAddr.sin_addr)));
}

void Server::handleClientMsg(int fd)
{
	std::string	message;
	bool		disconnected;
	bool		dispatch;

	Client& sender = getClientByFd(clients, fd);;

	disconnected = sender.recvMsg(message);
	if ( disconnected )
	{
		if (!message.empty())
			send(sender.getFd(), message.c_str(), message.size(), 0);
		eraseClient(sender);
		return;
	}

	dispatch = sender.handleMultipleMsg();
	if (dispatch && !sender.getCommand().empty())
		dispatcher.dispatch(sender);
}

void Server::eraseClient(Client &client)
{
	int client_fd = client.getFd();

	if (client.isRegistered())
		Server::removeClientFromChannels(client, "QUIT");

	std::vector<Client>::iterator toRemove = std::find(clients.begin(), clients.end(), client);
	clients.erase(toRemove);

	std::vector<pollfd>::iterator it = std::find(fds.begin(), fds.end(), client_fd);
	fds.erase(it);
	close(client_fd);
}

std::string	 Server::getPassword()
{
	return password;
}

std::vector<Client>	 Server::getClients()
{
	return clients;
}

Client*	 Server::getClient(std::string nickname)
{
	std::vector<Client>::iterator it = std::find(clients.begin(), clients.end(), nickname);
	if (it == clients.end())
		return NULL;
	else
		return &(*it);
}

Channel*	 Server::getChannel(std::string name)
{
	std::vector<Channel>::iterator it = std::find(channels.begin(), channels.end(), name);
	if (it == channels.end())
		return NULL;
	else
		return &(*it);
}

void  Server::addChannel(Channel& channel)
{
	channels.push_back(channel);
}

void Server::broadcastToUsers(std::string& message)
{
	std::vector<Client>::iterator end = clients.end();
	for (std::vector<Client>::iterator it = clients.begin(); it != end; it++)
	{
		if (it->isRegistered())
			send(it->getFd(), message.c_str(), message.size(), 0);
	}
}

void  Server::removeClientFromChannels(Client& client, std::string flag)
{
	std::string message;
	if (flag == "QUIT")
		message = RPL_QUIT(client.getMask(), "Client exited");
	
	for (std::vector<Channel>::iterator it = channels.begin(); it != channels.end();)
	{
		if (!it->isAMember(client.getFd()))
		{
			it++;
			continue;
		}
		it->removeClient(client.getFd());
		client.userQuitAChannel();
		if (flag == "PART")
		{
			message = RPL_PART(client.getMask(), it->getName() ,"");
			it->broadcast(message, -1);
		}
		if (it->getMembersSize() == 0)
			it = channels.erase(it);
		else
			it++;
	}
	if (flag == "QUIT" && client.isRegistered())
		Server::broadcastToUsers(message);
}

struct tm* Server::geTimeInfo()
{
	return timeinfo;
}

void Server::removeChannel(std::string& channelName)
{
	std::vector<Channel>::iterator it = std::find(channels.begin(), channels.end(), channelName);

	if (it != channels.end())
		channels.erase(it);
}

std::string Server::getClientNicknameByFd(int fd)
{
	std::vector<Client>::iterator it = std::find(clients.begin(), clients.end(), fd);
	if (it == clients.end())
		return "";
	else
		return it->getNickname();
}
