#include "Commands.hpp"
 JoinCommand::JoinCommand(Client& client) : Command(client) {}

std::vector<std::string> parseParameter(std::string& parameter)
{
	std::string message;
	std::string target;
	std::stringstream targets(parameter);
	std::vector<std::string> vec;

	while (std::getline(targets, target, ','))
			vec.push_back(target);
	return vec;
}

bool JoinCommand::isValidChannelName(std::string& channelName)
{
	if (channelName.empty() || channelName[0] != '#' || channelName.size() > CHANNELLEN)
		return false;

	for (size_t i = 0; i < channelName.length(); ++i)
	{
		char c = channelName[i];
        if (c == ' ' || c == ',' || c == ':' || c == '\a')
            return false;
    }
    return true;
}

void JoinCommand::sendJoinMessages(Channel* channel, std::string& channelName)
{
	std::string message;

	message = RPL_JOIN(client.getMask(), channelName);
	send(client.getFd(), message.c_str(), message.length(), 0);

	if (channel->getTopic().empty())
		message = RPL_NOTOPIC(client.getNickname(), channelName);
	else
		message = RPL_TOPIC(client.getNickname(), channelName, channel->getTopic());
	send(client.getFd(), message.c_str(), message.length(), 0);

	std::string joinedUsers = channel->getMembersWithRoles();
	size_t maxChunkSize = 390;
	while (!joinedUsers.empty())
	{
		size_t lastSpace = joinedUsers.rfind(' ', maxChunkSize);
		if (joinedUsers.size() <= maxChunkSize || lastSpace == std::string::npos)
			lastSpace = joinedUsers.size();
		std::string chunk = joinedUsers.substr(0, lastSpace);
		message = RPL_NAMREPLY(client.getNickname(), channelName, chunk);
		send(client.getFd(), message.c_str(), message.length(), 0);
		joinedUsers.erase(0, lastSpace + 1);
	}

	message = RPL_ENDOFNAMES(client.getNickname(), channelName);
	send(client.getFd(), message.c_str(), message.length(), 0);
}

std::string JoinCommand::joinChannel(std::string& channelName, std::string key)
{
	Channel* channel = Server::getChannel(channelName);

	
	if (channel == NULL)
	{
		if (!isValidChannelName(channelName))
			return ERR_BADCHANMASK(client.getNickname(), channelName);
		Channel newChannel(channelName);
		newChannel.addMember(client.getFd());
		newChannel.addMemberToOps(client.getFd());
		Server::addChannel(newChannel);
		sendJoinMessages(&newChannel, newChannel.getName());
		return "";
	}

	if (channel->isAMember(client.getNickname()))
		return "";

	if (channel->hasMode('i') && !channel->isInvited(client.getFd()))
		return ERR_INVITEONLYCHAN(client.getNickname(), channelName);

	if (channel->hasMode('k') && channel->isWrongKey(key))
		return ERR_BADCHANNELKEY(client.getNickname(), channelName);

	if (channel->isChannelFull())
		return ERR_CHANNELISFULL(client.getNickname(), channelName);

	if (client.getJoinedChannels() >= CHANLIMIT)
		return ERR_TOOMANYCHANNELS(client.getNickname(), channelName);

	channel->addMember(client.getFd());
	client.userJoinedAChannel();
	std::string message = RPL_JOIN(client.getMask(), channelName);
	channel->broadcast(message, client.getFd());
	sendJoinMessages(channel, channelName);
	return "";
}

bool JoinCommand::joinChannels(std::vector<std::string>& channels, std::vector<std::string>& keys)
{
	std::string message;
	size_t size = channels.size();
	for (size_t i = 0; i < size; i++)
	{
		if (i < keys.size())
			message = joinChannel(channels[i], keys[i]);
		else
			message = joinChannel(channels[i], "");
		if (!message.empty())		
			send(client.getFd(), message.c_str(), message.length(), 0);
	}
	return true;
}

bool JoinCommand::checkParameters()
{
	std::string message;

	if (paramsVec.size() == 1 && paramsVec.back() == "0")
	{
		Server::removeClientFromChannels(client, "PART");
		return true;
	}
	if (paramsVec.size() < 1 || paramsVec.size() > 2)
	{
		message =  ERR_NEEDMOREPARAMS(client.getNickname(), "JOIN");
        send(client.getFd(), message.c_str(), message.length(), 0);
        return false;
	}
	std::vector<std::string> channels = parseParameter(paramsVec[0]);
	std::vector<std::string> keys;
	if (paramsVec.size() == 2)
		keys = parseParameter(paramsVec[1]);
	joinChannels(channels, keys);
	return true;
}

void JoinCommand::execute()
{
	std::string message;

	if (!client.isRegistered())
	{
		message = ERR_NOTREGISTERED(client.getNickname());
        send(client.getFd(), message.c_str(), message.length(), 0);
		return ;
	}
	checkParameters();
}