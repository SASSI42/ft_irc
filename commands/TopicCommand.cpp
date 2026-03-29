#include "Commands.hpp"

TopicCommand::TopicCommand(Client& client) : Command(client) {}

void TopicCommand::broadcastTopicChange(Channel& channel )
{
	std::string topicChange;

	channel.setTopic(paramsVec[1]);
	topicChange = RPL_NEWTOPIC(client.getMask(), channel.getName(), paramsVec[1]);
	channel.broadcast(topicChange, -1);
}

bool TopicCommand::checkParameters()
{
	std::string message;

	if (paramsVec.empty())
	{
		message = ERR_NEEDMOREPARAMS(client.getNickname(), "TOPIC");
		send(client.getFd(), message.c_str(), message.length(), 0);
		return (false);
	}
	Channel* channel = Server::getChannel(paramsVec[0]);
	if (channel == NULL)
	{
		message = ERR_NOSUCHCHANNEL(client.getNickname(), paramsVec[0]);
		send(client.getFd(), message.c_str(), message.length(), 0);
		return (false);
	}
	if (!channel->isAMember(client.getFd()))
	{
		message = ERR_NOTONCHANNEL(client.getNickname(), paramsVec[0]);
		send(client.getFd(), message.c_str(), message.length(), 0);
		return (false);
	}
	if (paramsVec.size() == 1)
	{
		if (channel->getTopic().empty())
		{
			message = RPL_NOTOPIC(client.getNickname(), paramsVec[0]);
			send(client.getFd(), message.c_str(), message.length(), 0);
			return true;
		}
		message = RPL_TOPIC(client.getNickname(), paramsVec[0], channel->getTopic());
		send(client.getFd(), message.c_str(), message.length(), 0);
		return true;
	}
	if (channel->hasMode('t') && !channel->isAnOperator(client.getFd()))
	{
		message = ERR_CHANOPRIVSNEEDED(client.getNickname(), paramsVec[0]);
		send(client.getFd(), message.c_str(), message.length(), 0);
		return (false);
	}
	broadcastTopicChange(*channel);
	return true;
}

void TopicCommand::execute()
{
	if (!client.isRegistered())
	{
		std::string message = ERR_NOTREGISTERED(client.getNickname());
		send(client.getFd(), message.c_str(), message.length(), 0);
		return ;
	}
	checkParameters();
}