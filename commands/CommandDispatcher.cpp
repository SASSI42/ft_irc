#include "CommandDispatcher.hpp"

CommandDispatcher::CommandDispatcher()
{
	_handlers["PASS"] = &CommandDispatcher::handlePass;
	_handlers["NICK"] = &CommandDispatcher::handleNick;
	_handlers["USER"] = &CommandDispatcher::handleUser;
	_handlers["MODE"] = &CommandDispatcher::handleMode;
	_handlers["TOPIC"] = &CommandDispatcher::handleTopic;
	_handlers["KICK"] = &CommandDispatcher::handleKick;
	_handlers["PRIVMSG"] = &CommandDispatcher::handlePrivmsg;
	_handlers["INVITE"] = &CommandDispatcher::handleInvite;
	_handlers["JOIN"] = &CommandDispatcher::handleJoin;
}

std::string CommandDispatcher::toUpperCase(const std::string& input)
{
    std::string result = input;
    for (size_t i = 0; i < result.size(); ++i) {
        result[i] = std::toupper(static_cast<unsigned char>(result[i]));
    }
    return result;
}

void CommandDispatcher::dispatch(Client& client)
{
	const std::string& cmd = toUpperCase(client.getCommand());

	std::map<std::string, CommandHandler>::iterator it = _handlers.find(cmd);
	if (it != _handlers.end())
		(this->*(it->second))(client);
	else
	{
		std::string message  = ERR_UNKNOWNCOMMAND(client.getNickname(), client.getCommand());
		send(client.getFd(), message.c_str(), message.size(), 0);
	}
}

void CommandDispatcher::handlePass(Client& client)
{
	PassCommand cmd(client);
	cmd.execute();
}
void CommandDispatcher::handleUser(Client& client)
{
	UserCommand cmd(client);
	cmd.execute();
}
void CommandDispatcher::handleNick(Client& client)
{
	NickCommand cmd(client);
	cmd.execute();
}
void CommandDispatcher::handleInvite(Client& client)
{
	InviteCommand cmd(client);
	cmd.execute();
}
void CommandDispatcher::handleJoin(Client& client)
{
	JoinCommand cmd(client);
	cmd.execute();
}
void CommandDispatcher::handleTopic(Client& client)
{
	TopicCommand cmd(client);
	cmd.execute();
}
void CommandDispatcher::handleMode(Client& client)
{
	ModeCommand cmd(client);
	cmd.execute();
}
void CommandDispatcher::handlePrivmsg(Client& client)
{
	PrivmsgCommand cmd(client);
	cmd.execute();
}
void CommandDispatcher::handleKick(Client& client)
{
	KickCommand cmd(client);
	cmd.execute();
}
