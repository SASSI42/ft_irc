#include "Channel.hpp"

void Channel::removeClient(int client)
{
	if (client < 0)
		return;
	this->operators.erase(client);
	this->invitedClients.erase(client);
	this->membersFd.erase(client);
}

std::string Channel::getMembersWithRoles()
{
	std::string  users;
	std::set<int>::iterator end = membersFd.end();
	for (std::set<int>::iterator it = membersFd.begin(); it != end; it++)
	{
		if (isAnOperator(*it))
			users +=  "@";
		users += Server::getClientNicknameByFd(*it) + " ";
	}
	if (!users.empty())
		users.pop_back();
	return users;
}

void Channel::SetInviteOnly(std::string param, Client client)
{
	std::string message = RPL_MODE(client.getMask(), name, "+i " + param);
	broadcast(message, -1);

	modes.insert('i');
}

void Channel::RemoveInviteOnly(std::string param, Client client)
{
	std::string message = RPL_MODE(client.getMask(), name, "-i " + param);
	broadcast(message, -1);

	modes.erase('i');
}

void Channel::SetTopicRestriction(std::string param, Client client)
{
	std::string message = RPL_MODE(client.getMask(), name, "+t " + param);
	broadcast(message, -1);
	modes.insert('t');

}

void Channel::RemoveTopicRestriction(std::string param, Client client)
{
	std::string message = RPL_MODE(client.getMask(), name, "-t " + param);
	broadcast(message, -1);
	modes.erase('t');
}

bool Channel::isInvalideKey(std::string key)
{
	if (key.empty() || key.size() > 23)
		return true;
	for (size_t i = 0; i < key.size(); ++i)
	{
		unsigned char ch = key[i];
		 
		if (ch <= 32 || ch == ','|| ch == ':' || ch == 127)
        	return true;
	}
	return false;
}
void Channel::SetPassword(std::string param, Client client)
{
	std::string message;
	if (isInvalideKey(param))
	{
		message = ERR_INVALIDKEY(client.getNickname(), name);
		send(client.getFd(), message.c_str(), message.length(), 0);
		return;
	}
	message = RPL_MODE(client.getMask(), name, "+k");
	broadcast(message, client.getFd());
	message = RPL_MODE(client.getMask(), name, "+k " + param);
	send(client.getFd(), message.c_str(), message.length(), 0);
	modes.insert('k');
	password = param;
}

void Channel::RemovePassword(std::string param, Client client)
{
	std::string message = RPL_MODE(client.getMask(), name, "-k" + param);
	broadcast(message, -1);
	modes.erase('k');
	password.clear();
}

void Channel::GiveOperatorPrivilege(std::string param, Client client)
{
	std::string message;
	if (!isAMember(param))
	{
		if (param.size() > 300)
			param = param.substr(0, 300);
		message = ERR_USERNOTINCHANNEL(client.getNickname(), param, name);
		send(client.getFd(), message.c_str(), message.length(), 0);
		return;
	}
	message = RPL_MODE(client.getMask(), name, "+o " + param);
	broadcast(message, -1);
	operators.insert(getMemberFd(param));
}

void Channel::TakeOperatorPrivilege(std::string param, Client client)
{
	std::string message;
	int targetFd = getMemberFd(param);
	if (targetFd < 0)
	{
		message = ERR_USERNOTINCHANNEL(client.getNickname(), param, name);
		send(client.getFd(), message.c_str(), message.length(), 0);
		return;
	}
	message = RPL_MODE(client.getMask(), name, "-o " + param);
	broadcast(message, -1);
	operators.erase(targetFd);
}
void Channel::SetUserLimit(std::string param, Client client)
{
	std::stringstream ss(param);
	int number = 0;

	if (!(ss >> number) || number <= 0)
	{
		if (param.size() > 300)
			param = param.substr(0, 300);
		std::string message = ERR_INVALIDMODEPARAM(client.getNickname(), name, "+l", param, "+l requires a strictly positive number");
		send(client.getFd(), message.c_str(), message.length(), 0);
		return;
	}
	std::string message = RPL_MODE(client.getMask(), name, "+l " + param);
	broadcast(message, -1);
	membersLimit = number;
	modes.insert('l');
}

void Channel::RemoveUserLimit(std::string param, Client client)
{
	std::string message = RPL_MODE(client.getMask(), name, "-l " + param);
	broadcast(message, -1);
	modes.erase('l');
	membersLimit = -1;
}

bool Channel::isAMember(int fd)
{
	if (membersFd.find(fd) != membersFd.end())
		return true;
	return false;
}

bool Channel::isAMember(std::string nickname)
{
	if (getMemberFd(nickname) < 0)
		return false;
	return true;
}

void Channel::addMember(int newMember)
{
	membersFd.insert(newMember);
}

void Channel::addMemberToOps(int fd)
{
	operators.insert(fd);
}

bool Channel::isAnOperator(int fd)
{
	return operators.find(fd) != operators.end();
}

void Channel::setTopic(std::string& newTopic)
{
	topic = newTopic.substr(0, TOPICLEN);
}

bool Channel::hasMode(char mode)
{
	return modes.find(mode) != modes.end();
}

bool Channel::isInvited(int fd)
{
	return invitedClients.find(fd) != invitedClients.end();
}

bool Channel::isChannelFull()
{
	return membersFd.size() >= membersLimit;
}

bool Channel::isWrongKey(std::string key)
{
	return password != key;
}

int Channel::getMemberFd(std::string nickname)
{
	for (std::set<int>::iterator it = membersFd.begin(); it != membersFd.end(); ++it)
	{
		if (Server::getClientNicknameByFd(*it) == nickname)
    		return (*it);
	}
    return -1;
}

void	Channel::addInvitedClients(int fd)
{
	invitedClients.insert(fd);
}

void	Channel::broadcast(std::string& message, int exception)
{
	for (std::set<int>::iterator it = membersFd.begin(); it != membersFd.end(); it++)
	{
		int target = *it;
		if (exception == target)
			continue;
		send(target, message.c_str(), message.size(), 0);
	}
}

bool	Channel::operator==(const std::string& name)
{
	return this->name == name ;
}

std::string& Channel::getTopic()
{
	return topic;
}

std::string& Channel::getName()
{
	return name;
}

Channel::Channel(std::string& Newname)
{
	name = Newname;
}

Channel::~Channel() {}

size_t Channel::getMembersSize()
{
	return membersFd.size();
}

std::set<int> Channel::getMembers()
{
	return membersFd;
}