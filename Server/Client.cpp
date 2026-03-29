#include "Client.hpp"

Client::Client(int _fd, std::string ip) : fd(_fd), clientAddr(ip)
{
	nickname = "*";
	joinedChannels = 0; 
	authenticated = false;
	registered = false;
}

Client &Client::operator=(Client const &copy)
{
	this->fd = copy.fd;
	this->nickname = copy.nickname;
	this->username = copy.username;
	this->realname = copy.realname;
	this->registered = copy.registered;
	this->authenticated = copy.authenticated;
	return (*this);
}

bool	Client::operator==(const int& fd)
{
	return this->fd == fd;
}

bool	Client::operator==(const std::string& name)
{
	return this->nickname == name;
}

bool	Client::operator==(Client const &copy)
{
	return this->fd == copy.fd;
}

int		Client::getFd()
{
	return fd; 
}

bool Client::recvMsg(std::string &message)
{
	int		bytes;
	char	buf[LINEMAX];

	//receive raw data fom client
	bytes = recv(this->fd, buf, sizeof(buf) - 1, 0);
	if (bytes <= 0)
		return true;
	buf[bytes] = '\0';
	//join raw data to bufer and check its size
	this->buffer.append(buf, bytes);
	if ( this->buffer.size() > BUFFERMAX ) {
		message = "ERROR :Input buffer overflow. You have been disconnected.\r\n";
		return true;
	}

	return false;
}

bool Client::handleMultipleMsg()
{
	std::string	message;
	int			len = 2;
	size_t			pos;

	pos = buffer.find("\r\n");
	if ( pos == std::string::npos) {
		pos = buffer.find("\n");
		if (pos == std::string::npos)
			return false;
		len = 1;
	}
	while (pos != std::string::npos)
	{
		//get message from buffer 
		message = this->buffer.substr(0, pos);
		this->buffer.erase(0, pos + len);
		parseMessage(message);
		len = 2;
		pos = buffer.find("\r\n");
		if ( pos == std::string::npos) {
			pos = buffer.find("\n");
			len = 1;
		}
	}
	return true;
}

void Client::parseMessage(std::string &message)
{
	std::string		param;
	std::string		last_param;
	size_t			pos;

	// std::cout << "parsedCmd.params.clear();" << std::endl;
	parsedCmd.params.clear();
	//find the first space to extract the command
	pos = message.find(" ");
	if (pos == std::string::npos)
	{
		parsedCmd.command = message;
		return;
	}
	parsedCmd.command = message.substr(0, pos);
	message.erase(0, pos);
	//find the : to extract the trailing param
	pos = message.find(" :");
	if (pos != std::string::npos)
	{
		pos+=2; //we add 2 to start the trailing param right after :
		last_param = message.substr(pos);
		message.erase(pos - 2);
	}

	//split by spaces to extract the in between params
	std::stringstream	strm(message); 

	while ( strm >> param )
		parsedCmd.params.push_back(param);

	if ( !last_param.empty() )
		parsedCmd.params.push_back(last_param);
}

void Client::userJoinedAChannel()
{
	joinedChannels++;
}

void Client::userQuitAChannel()
{
	if (joinedChannels > 0)
		joinedChannels--;
}

int Client::getJoinedChannels()
{
	return joinedChannels;
}

bool Client::isAuthenticated()
{
	return authenticated;
}

void Client::setAuthentification(bool state)
{
	authenticated = state;
}


void Client::welcomeUser()
{
	std::string message = RPL_WELCOME(nickname, username, clientAddr);
	send(fd, message.c_str(), message.size(), 0);
	message = RPL_YOURHOST(nickname);
	send(fd, message.c_str(), message.size(), 0);
	char creationTime[100];
	strftime(creationTime, sizeof(creationTime), "%a %b %d %H:%M:%S %Y", Server::geTimeInfo());
	message = RPL_CREATED(nickname, creationTime);
	send(fd, message.c_str(), message.size(), 0);
	message = RPL_MYINFO(nickname);
	send(fd, message.c_str(), message.size(), 0);
}

void Client::setRegistration(bool state)
{
	registered = state;
}

bool Client::isRegistered()
{
	return registered;
}

void Client::setNickname(std::string nickname)
{
	this->nickname = nickname.substr(0, NICKLEN);
}

void Client::setUsername(std::string username)
{
	  this->username = username.substr(0, USERLEN);
}

void Client::setRealname(std::string realname)
{
	this->realname = realname.substr(0, USERLEN);
}

std::string Client::getNickname()
{
	return nickname;
}

std::string Client::getUsername()
{
	return username;
}

std::string Client::getCommand()
{
	return parsedCmd.command;
}

std::vector<std::string> Client::getParamsVec()
{
	return parsedCmd.params;
}

std::string Client::getClientAddr()
{
	return clientAddr;
}
void Client::setClientAddr(std::string ipAddr)
{
	clientAddr = ipAddr;
}

std::string Client::getMask()
{
	return std::string(":") + nickname + std::string("!") + username + std::string("@") +  clientAddr; 
}
