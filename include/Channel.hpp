#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"
#include "Client.hpp"
#include <string>
#include <map>

enum ChannelMode
{
    INVITE_ONLY = 0,
    PASSWORD    = 1,
    OPS   		= 2,
    LIMIT       = 3,
    TOPIC       = 4,
	LIST_MODE	= 5
};



class Server;
class Client;

class Channel
{
	private:
		const Server			_server;
		const std::string		_name;
		const std::string		_password;
		std::string				_topic;
		int						_modes; //bitmask in order p/s/i/t/n/m
		int						_userLimit;
		std::map<Client, int>	_memberList;//client 0 = not connected/ client 1 = connected/ client 2 = invited
		bool					_modList[LIST_MODE] = {false};
		//todo banlist container
		
		void	join(Client user);
		void	setMode(bool isRemoved, int mask);
		//void	setUserMode(Client user, bool isRemoved, int mask);
		void	invite(Client user);

	public:
		Channel();
		~Channel();
		Channel(Server serv, Client cli, std::string name);

		void kick(Client issuer, Client target);
		void mode(Client user, std::string modes);
		void names(Client user);
		void topic(Client user, std::string newTopic);
		void Channel::topic(Client user);
		void part(Client user);
		void join(Client user, std::string password);
		std::string& getName();
		std::string& getTopic();
		std::string& getpassword();
		void setTopic(std::string topic);
		bool*	whichMod();
		bool	isInvited(Client &client);
};

#endif
