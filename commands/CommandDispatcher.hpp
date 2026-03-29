#ifndef COMMANDDISPATCHER_HPP
#define COMMANDDISPATCHER_HPP

#include <map>
#include <string>
#include <vector>
#include "../Server/Client.hpp"
#include "../Server/replies.hpp"
#include "Commands.hpp"

class Command;
class PassCommand;
class NickCommand;
class InviteCommand;
class JoinCommand;
class KickCommand;
class ModeCommand;
class PrivmsgCommand;
class TopicCommand;
class UserCommand;

class CommandDispatcher
{
	public:
		typedef void (CommandDispatcher::*CommandHandler)(Client&);

		CommandDispatcher();
		void dispatch(Client& client);

	private:

		std::map<std::string, CommandHandler> _handlers;

		std::string toUpperCase(const std::string& input);
		void handlePass(Client& client);
		void handleUser(Client& client);
		void handleNick(Client& client);
		void handleInvite(Client& client);
		void handleJoin(Client& client);
		void handleTopic(Client& client);
		void handleMode(Client& client);
		void handlePrivmsg(Client& client);
		void handleKick(Client& client);
};

#endif