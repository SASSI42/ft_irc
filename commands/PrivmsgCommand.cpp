#include "Commands.hpp"

PrivmsgCommand::PrivmsgCommand(Client& client) : Command(client) {}

void PrivmsgCommand::sendMessageToChannel(std::string& channelName, std::string& message)
{
	Channel* channel = Server::getChannel(channelName);
	if (channel == NULL)
	{
		message = ERR_NOSUCHCHANNEL(client.getNickname(), channelName);
		send(client.getFd(), message.c_str(), message.length(), 0);
		return ;
	}
	if (!channel->isAMember(client.getFd()))
	{
		message = ERR_CANNOTSENDTOCHAN(client.getNickname(), channelName);
		send(client.getFd(), message.c_str(), message.length(), 0);
		return ;
	}
	message = RPL_PRIVMSG(client.getMask(), channelName, message);
	channel->broadcast(message, client.getFd());
}

void PrivmsgCommand::sendMessageToUser(std::string& target, std::string& message)
{
	Client* targetUser = Server::getClient(target);
	if (targetUser == NULL)
	{
		message = ERR_NOSUCHNICK(client.getNickname(), target);
		send(client.getFd(), message.c_str(), message.length(), 0);
		return ;
	}
	message = RPL_PRIVMSG(client.getMask(), target, message);
	send(targetUser->getFd(), message.c_str(), message.length(), 0);
}

bool PrivmsgCommand::checkParameters()
{
	std::string message;

	if (paramsVec.size() < 2)
	{
		message = ERR_NEEDMOREPARAMS(client.getNickname(), "PRIVMSG");
		send(client.getFd(), message.c_str(), message.length(), 0);
		return false;
	}
	std::stringstream targets(paramsVec[0]);
	std::string target;
	while (std::getline(targets, target, ','))
	{
		if (!target.empty() && target[0] == '#')
			sendMessageToChannel(target, paramsVec[1]);
		else
			sendMessageToUser(target, paramsVec[1]);
	}
	return true;
}

void PrivmsgCommand::execute()
{
	if (!client.isRegistered())
	{
		std::string message = ERR_NOTREGISTERED(client.getNickname());
		send(client.getFd(), message.c_str(), message.length(), 0);
		return ;
	}
	checkParameters();
}