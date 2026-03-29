#include "Commands.hpp"

PassCommand::PassCommand(Client& client) : Command(client) {}

bool PassCommand::checkParameters()
{
	 std::string message;

	if (client.isRegistered() == 1)
	{
        message = ERR_ALREADYREGISTERED(client.getNickname());
        send(client.getFd(), message.c_str(), message.length(), 0);
        return (false);
    }
	if (paramsVec.empty())
	{
		message = ERR_NEEDMOREPARAMS(client.getNickname(), "PASS");
        send(client.getFd(), message.c_str(), message.length(), 0);
		return false;
	}
    if (paramsVec[0] != Server::getPassword())
	{
        message = ERR_PASSWDMISMATCH(client.getNickname());
        send(client.getFd(), message.c_str(), message.length(), 0);
        client.setAuthentification(false);
        return (false);
    }
	return true;
}

void PassCommand::execute()
{
	if (checkParameters())
		client.setAuthentification(true);
}