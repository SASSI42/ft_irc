#include "Commands.hpp"

InviteCommand::InviteCommand(Client& client) : Command(client) {}

bool InviteCommand::checkParameters()
{
	 std::string message;

	if (paramsVec.size() != 2)
	{
        message = ERR_NEEDMOREPARAMS(client.getNickname(), "INVITE");
        send(client.getFd(), message.c_str(), message.length(), 0);
        return false;
    }
	channel = Server::getChannel(paramsVec[1]);
	if (channel == NULL)
	{
        message = ERR_NOSUCHCHANNEL(client.getNickname(), paramsVec[1]);
        send(client.getFd(), message.c_str(), message.length(), 0);
        return false;
    }
	invitedUser = paramsVec[0];
	if (!channel->isAMember(client.getNickname()))
	{
        message = ERR_NOTONCHANNEL(client.getNickname(), channel->getName());
        send(client.getFd(), message.c_str(), message.length(), 0);
        return false;
    }
	if (channel->hasMode('i') && !channel->isAnOperator(client.getFd()))
	{
        message = ERR_CHANOPRIVSNEEDED(client.getNickname(), channel->getName());
        send(client.getFd(), message.c_str(), message.length(), 0);
        return false;
    }
	if (channel->isAMember(invitedUser))
	{
        message = ERR_USERONCHANNEL(client.getNickname(), invitedUser, channel->getName());
        send(client.getFd(), message.c_str(), message.length(), 0);
        return false;
    }

	return true;
}

void InviteCommand::execute()
{
	std::string message;

	if (!client.isRegistered())
	{
		std::string message = ERR_NOTREGISTERED(client.getNickname());
		send(client.getFd(), message.c_str(), message.length(), 0);
		return ;
	}
	if (checkParameters())
	{
		Client* invitedClient = Server::getClient(invitedUser);
		if (invitedClient == NULL)
		{
			message = ERR_NOSUCHNICK(client.getNickname(), invitedUser);
       		send(client.getFd(), message.c_str(), message.length(), 0);
			return ;
		}
		message = RPL_INVITE(client.getMask(), invitedUser, channel->getName());
        send(invitedClient->getFd(), message.c_str(), message.length(), 0);
		channel->addInvitedClients(invitedClient->getFd());
		message = RPL_INVITING(client.getNickname(), invitedUser, channel->getName());
        send(client.getFd(), message.c_str(), message.length(), 0);
	}
}