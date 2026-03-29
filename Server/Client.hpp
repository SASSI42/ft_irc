#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <set>
#include <sys/socket.h>
#include <stdexcept>
#include "Channel.hpp"
#include "replies.hpp"
#include "Server.hpp"
#include <sstream>
#include "utils.hpp"

#define USERLEN 20
#define NICKLEN 15
#define TOPICLEN 300


class Client {
	private :
		int						fd;
		std::string				nickname;
		std::string				clientAddr;
		std::string				username;
		std::string				realname;
		parsedCommand			parsedCmd;
		bool					registered;
		bool					authenticated;
		std::string				buffer;
		int						joinedChannels;
		Client();

	public :
		std::string					getClientAddr();
		std::string					getMask();
		void						setClientAddr(std::string ipAddr);
		int							getJoinedChannels();
		void						userJoinedAChannel();
		void						userQuitAChannel();
		bool						isAuthenticated();
		void						setAuthentification(bool state);
		void						setRegistration(bool state);
		bool						isRegistered();
		void						welcomeUser();
		void						setNickname(std::string nickname);
		void						setUsername(std::string username);
		void						setRealname(std::string realname);
		std::string					getNickname();
		std::string					getUsername();
		std::string					getCommand();
		std::vector<std::string>	getParamsVec();

		Client(int fd, std::string ip);
		
		Client	&operator=(Client const &copy);
		bool	operator==(const int& fd);
		bool	operator==(const std::string& name);
		bool	operator==(Client const &copy);
		int		getFd();
		bool	recvMsg(std::string &message);
		bool	handleMultipleMsg();
		void	parseMessage(std::string &message);
};

#endif