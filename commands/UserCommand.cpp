#include "Commands.hpp"

UserCommand::UserCommand(Client& client) : Command(client) {}

bool UserCommand::isEmpty(std::string& str)
{
	for (size_t i = 0; i < str.size(); ++i)
	{
		if (!std::isspace(static_cast<unsigned char>(str[i])))
			return false;
	}
	return true;
}

bool UserCommand::checkParameters()
{
	std::string message;
	if (client.isRegistered())
	{
		message = ERR_ALREADYREGISTERED(client.getNickname());
		send(client.getFd(), message.c_str(), message.length(), 0);
		return (false);
	}
	if (paramsVec.size() != 4 || isEmpty(paramsVec[0]))
	{
		message = ERR_NEEDMOREPARAMS(client.getNickname(), "USER");
		send(client.getFd(), message.c_str(), message.length(), 0);
		return (false);
	}
	if (paramsVec[1] != "0" || paramsVec[2] != "*" )
	{
		message = ERR_NEEDMOREPARAMS(client.getNickname(), "USER");
		send(client.getFd(), message.c_str(), message.length(), 0);
		return (false);
	}
	return true;
}

void UserCommand::execute()
{
	if (!client.isAuthenticated())
		return ;
	if (checkParameters())
	{
		std::string nickname = client.getNickname();
		client.setUsername(client.getParamsVec()[0]);
		client.setRealname(client.getParamsVec()[3]);
		if (nickname != "*")
		{
			client.setRegistration(true);
			client.welcomeUser();
		}
	}
}