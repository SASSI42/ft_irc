#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#define CHANLIMIT 25
#define CHANNELLEN 51

#include "../Server/Client.hpp"
#include "../Server/Channel.hpp"
#include "../Server/Server.hpp"
#include "../Server/replies.hpp"

class Command
{
	protected:
		Client&						client;
		std::vector<std::string>	paramsVec;

	public:
		virtual void execute() = 0;
		Command(Client& client);
		virtual ~Command();
};

class PassCommand : public Command
{
    private:
        bool checkParameters();

    public: 
        void execute();
        PassCommand(Client& client);
};

class NickCommand : public Command
{
    private:
        bool checkParameters();
        bool checkNickname();

    public:
        void execute();
        NickCommand(Client& client);
};

class UserCommand : public Command
{
    private:
		bool isEmpty(std::string& str);
        bool checkParameters();

    public:
        void execute();
        UserCommand(Client& client);
};

class JoinCommand : public Command
{
	private:
		std::string joinChannel(std::string& channelName, std::string key);
		bool 		joinChannels(std::vector<std::string>& channels, std::vector<std::string>& keys);
		void 		sendJoinMessages(Channel* channel, std::string& channelName);
		bool 		isValidChannelName(std::string& channelName);
		bool		checkParameters();

	public:
		void execute();
		JoinCommand(Client& client);
};

class TopicCommand : public Command
{
	private:
		bool checkParameters();
		void broadcastTopicChange(Channel& channel);

	public:
		void execute();
		TopicCommand(Client& client);
};

class KickCommand : public Command 
{
	private:
		std::vector<std::string>	validTargets;
		bool						checkParameters();
		void						getValidTargets(Channel& channel);


	public:
		void execute();
		KickCommand(Client& client);
};

class InviteCommand : public Command
{
	private:
		Channel*	channel;
		std::string invitedUser;
		bool		checkParameters();

	public:
		void execute();
		InviteCommand(Client& client);
};

class ModeCommand : public Command
{
	private:
		bool checkParameters();
		bool needsParam(char c, char sign);

	public:
		void execute();
		ModeCommand(Client& client);
};

class PrivmsgCommand : public Command
{
    private:
		void sendMessageToChannel(std::string& channelName, std::string& message);
		void sendMessageToUser(std::string& target, std::string& message);
        bool checkParameters();

    public:
        void execute();
        PrivmsgCommand(Client& client);
};

#endif
