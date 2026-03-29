#ifndef UTILS_H
#define UTILS_H

#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <string>
#include <iostream>
#include "Exceptions.hpp"
#include <string>
#include <limits.h>
#include <errno.h>

#define BUFFERMAX	4096
#define LINEMAX		512

typedef struct cmd {
	std::string					command;
	std::vector<std::string>	params;
} parsedCommand;

class Client;
class Channel;

void	signalHandlers(void);
bool	isValidPassword(std::string const &password);
bool	isValidPort(long port, char *delimiter);
void	addToFds(std::vector<pollfd> &fds, int fd);
Client	&getClientByFd(std::vector<Client> &clients, int fd);
bool	operator==(const pollfd &lhs,const pollfd &rhs);
bool	operator==(const pollfd &lhs, const int &rhs);

#include "Client.hpp"
#include "Channel.hpp"

#endif