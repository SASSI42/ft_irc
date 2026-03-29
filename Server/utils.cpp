#include "utils.hpp"

Client &getClientByFd(std::vector<Client> &clients, int fd)
{
	for (std::vector<Client>::iterator it = clients.begin(); it != clients.end(); it++)
	{
		if (it->getFd() == fd)
			return (*it);
	}
	throw RunException("Client not found !");
}

//join an fd to the pollfd vector
void addToFds(std::vector<pollfd> &fds, int fd)
{
	// std::cout << "\n" << fd << "has been added to fds\n"; //debug
	struct pollfd newfd;

	newfd.fd = fd;
	newfd.events = POLLIN;

	fds.push_back(newfd);
}

bool operator==(const pollfd &lhs, const pollfd &rhs)
{
	return( lhs.fd == rhs.fd );
}

//overload == for (pollfd, int) to use std::find
bool operator==(const pollfd &lhs, const int &rhs)
{
	return( lhs.fd == rhs );
}

bool	isValidPassword(std::string const &password)
{
	if ( password.size() > 400 || password.empty() )
		return false;
	for (size_t i = 0; i < password.size(); i++)
	{
		unsigned char c = password[i];
		if (!isprint((c)) || isspace((c)))
			return false;
	}
	return true;
}

bool isValidPort(long port, char *delimiter)
{
	if (errno == ERANGE || port > INT_MAX || port < INT_MIN )
		return false;
	if ( port < 1024 || port > 65535 || *delimiter != '\0' )
		return false;
	return true;
}

void signalHandlers(void)
{
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, Server::shutdown);
	signal(SIGQUIT, Server::shutdown);
}