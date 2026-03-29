#include "Commands.hpp"

KickCommand::KickCommand(Client& client) : Command(client) {}

void KickCommand::getValidTargets(Channel& channel)
{
	std::string message;
	std::stringstream targets(paramsVec[1]);
	std::string target;

	while (std::getline(targets, target, ','))
	{
		if (!channel.isAMember(target))
		{
			message = ERR_USERNOTINCHANNEL(client.getNickname(), target, channel.getName());
			send(client.getFd(), message.c_str(), message.length(), 0);
		}
		else
			validTargets.push_back(target);
	}
}

bool KickCommand::checkParameters()
{
	std::string message;

	if (paramsVec.size() != 2 &&  paramsVec.size() != 3)
	{
        message = ERR_NEEDMOREPARAMS(client.getNickname(), "KICK");
        send(client.getFd(), message.c_str(), message.length(), 0);
        return (false);
    }
	Channel* channel = Server::getChannel(paramsVec[0]);
	if (channel == NULL)
	{
		message = ERR_NOSUCHCHANNEL(client.getNickname(), paramsVec[0]);
        send(client.getFd(), message.c_str(), message.length(), 0);
		return false;
	}
	if (!channel->isAMember(client.getFd()))
	{
		message = ERR_NOTONCHANNEL(client.getNickname(), paramsVec[0]);
        send(client.getFd(), message.c_str(), message.length(), 0);
		return false;
	}
	if (!channel->isAnOperator(client.getFd()))
	{
		message = ERR_CHANOPRIVSNEEDED(client.getNickname(), paramsVec[0]);
        send(client.getFd(), message.c_str(), message.length(), 0);
		return false;
	}
	getValidTargets(*channel);
	return true;
}

void KickCommand::execute()
{
	std::string message;

	if (!client.isRegistered())
	{
		message = ERR_NOTREGISTERED(client.getNickname());
        send(client.getFd(), message.c_str(), message.length(), 0);
		return ;
	}
	if (checkParameters())
	{
		Channel* channel = Server::getChannel(paramsVec[0]);
		std::vector<std::string>::iterator end = validTargets.end();
		for (std::vector<std::string>::iterator it = validTargets.begin(); it != end; it++)
		{
			if (paramsVec.size() == 2)
				message = RPL_KICK(client.getMask(), paramsVec[0], *it, "No reason given");
			else
				message = RPL_KICK(client.getMask(), paramsVec[0], *it, paramsVec[2]);

			channel->broadcast(message, -1);
			channel->removeClient(channel->getMemberFd(*it));
			if (channel->getMembersSize() == 0)
				Server::removeChannel(channel->getName());
			Server::getClient(*it)->userQuitAChannel();
		}
	}
}