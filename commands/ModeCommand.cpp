#include "Commands.hpp"

ModeCommand::ModeCommand(Client& client) : Command(client) {}

bool ModeCommand::needsParam(char c, char sign)
{
    return (c == 'k' && sign == '+') || (c == 'l' && sign == '+') || c == 'o';
}

bool ModeCommand::checkParameters()
{
	std::string message;

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
	if (!channel->isAnOperator(client.getFd()))
	{
		message = ERR_CHANOPRIVSNEEDED(client.getNickname(), paramsVec[0]);
		send(client.getFd(), message.c_str(), message.length(), 0);
		return (false);
	}
	if (paramsVec.size() < 2)
	{
		message = ERR_NEEDMOREPARAMS(client.getNickname(), "MODE");
		send(client.getFd(), message.c_str(), message.length(), 0);
		return (false);
	}
	size_t modeArgumentIndex = 2;
	std::map<std::string, void (Channel::*)(std::string, Client)> mode;
	mode["+i"] = &Channel::SetInviteOnly;
    mode["-i"] = &Channel::RemoveInviteOnly;
    mode["+t"] = &Channel::SetTopicRestriction;
    mode["-t"] = &Channel::RemoveTopicRestriction;
    mode["+k"] = &Channel::SetPassword;
    mode["-k"] = &Channel::RemovePassword;
    mode["+o"] = &Channel::GiveOperatorPrivilege;
    mode["-o"] = &Channel::TakeOperatorPrivilege;
    mode["+l"] = &Channel::SetUserLimit;
    mode["-l"] = &Channel::RemoveUserLimit;
	char sign = '+';
	for (size_t i = 0; i < paramsVec[1].size(); i++)
	{
		std::string str;
		char c = paramsVec[1][i];
        if (c == '+') { sign = '+'; continue; }
        if (c == '-') { sign = '-'; continue; }
		str += sign;
		str += c;
		std::map<std::string, void (Channel::*)(std::string, Client)>::iterator it = mode.find(str);
		if (it == mode.end())
		{
			message = ERR_UNKNOWNMODE(client.getNickname(), str, channel->getName());
			send(client.getFd(), message.c_str(), message.length(), 0);
			continue;
		}
		std::string param;
		if (needsParam(c, sign))
		{
			if (paramsVec.size() <= modeArgumentIndex)
			{
				message = ERR_NEEDMOREPARAMS(client.getNickname(), "MODE");
				send(client.getFd(), message.c_str(), message.length(), 0);
				continue;
			}
			param = paramsVec[modeArgumentIndex];
			modeArgumentIndex++;
		}
		(channel->*it->second)(param, client);
	}
	return true;
}

void ModeCommand::execute()
{
	if (!client.isRegistered())
	{
		std::string message = ERR_NOTREGISTERED(client.getNickname());
		send(client.getFd(), message.c_str(), message.length(), 0);
		return ;
	}
	checkParameters();
}