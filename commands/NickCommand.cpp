#include "Commands.hpp"

NickCommand::NickCommand(Client& client) : Command(client) {}

bool NickCommand::checkNickname()
{
	std::string message;
	std::string nickname = paramsVec[0];
	std::string allowed = "[]{}\\|";

	for (size_t i = 0; i < nickname.size(); i++)
	{
		if ((allowed.find(nickname[i]) == std::string::npos && !isalnum(nickname[i])) || nickname.length() > NICKLEN || isdigit(nickname[0]))
		{
			message = ERR_ERRONEUSNICKNAME(client.getNickname(), nickname);
        	send(client.getFd(), message.c_str(), message.length(), 0);
			return false;
		}
	}

	std::vector<Client> clients = Server::getClients();
	std::vector<Client>::iterator end = clients.end();
	for (std::vector<Client>::iterator it = clients.begin(); it < end; it++)
	{
		if (it->getNickname() == nickname)
		{
			message = ERR_NICKNAMEINUSE(client.getNickname(), nickname);
        	send(client.getFd(), message.c_str(), message.length(), 0);
			return false;
		}
	}
	return true;
}

bool NickCommand::checkParameters()
{
	std::string message;
	if (paramsVec.empty())
	{
		message = ERR_NONICKNAMEGIVEN(client.getNickname());
		send(client.getFd(), message.c_str(), message.length(), 0);
		return false;
	}
	return true;
}

void NickCommand::execute()
{
	if (!client.isAuthenticated())
		return ;
	if (checkParameters() && checkNickname())
	{
		if (client.isRegistered())
		{
			std::string message = RPL_NICK(client.getMask(), paramsVec[0]);
			Server::broadcastToUsers(message);
		}
		client.setNickname(paramsVec[0]);
		if (!client.isRegistered() && !client.getUsername().empty())
		{
			client.setRegistration(true);
			client.welcomeUser();
		}
	}
}