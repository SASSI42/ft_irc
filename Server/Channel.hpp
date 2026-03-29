#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <vector>
#include <set>
#include "Client.hpp"
#include "replies.hpp"

class Client;
class Channel {
	private :
		std::string			name;
		std::string			password;
		std::set<int>		membersFd;
		unsigned	int		membersLimit;
		std::set<int>		operators;
		std::set<int>		invitedClients;
		std::string			topic;
		std::set<char>		modes;
	public :
		void SetInviteOnly(std::string param, Client client);
		void RemoveInviteOnly(std::string param, Client client);
		void SetTopicRestriction(std::string param, Client client);
		void RemoveTopicRestriction(std::string param, Client client);
		void SetPassword(std::string param, Client client);
		void RemovePassword(std::string param, Client client);
		void GiveOperatorPrivilege(std::string param, Client client);
		void TakeOperatorPrivilege(std::string param, Client client);
		void SetUserLimit(std::string param, Client client);
		void RemoveUserLimit(std::string param, Client client);

		bool isInvalideKey(std::string key);
		bool isAMember(int fd);
		bool isAMember(std::string nickname);
		void addMember(int newMember);
		void addMemberToOps(int fd);
		bool isAnOperator(int fd);
		void setTopic(std::string& newTopic);
		bool hasMode(char mode);
		bool isInvited(int fd);
		bool isChannelFull();
		bool isWrongKey(std::string key);
		int  getMemberFd(std::string nickname);

		std::set<int>	getMembers();
		size_t 			getMembersSize();
		std::string 	getMembersWithRoles();
		std::string 	&getTopic();
		bool			operator==(const std::string& name);
		void			addInvitedClients(int fd);
		std::string 	&getName();
		void			removeClient(int client);
		void			broadcast(std::string& message, int exception);

		Channel(std::string& Newname);
		~Channel();
};

#endif