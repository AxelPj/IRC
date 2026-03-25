#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"
#include "Client.hpp"
#include <string>
#include <map>

class Server;
class Client;

enum ChannelMode
{
    INVITE_ONLY = 0, //i
    PASSWORD    = 1, //k
    OPS   		= 2, //o
    LIMIT       = 3, //l
    TOPIC       = 4, //t
	LIST_MODE	= 5
};

enum ClientStatus
{
    NOT_CONNECTED 	= 0,
    CONNECTED    	= 1,
    INVITED   		= 2,
    OP       		= 3,
    BANNED	  		= 4
};

class Channel
{
	private:
		const Server			_server;
		const std::string		_name;
		const std::string		_password;
		std::string				_topic;
		int						_modes; //bitmask in order p/s/i/t/n/m
		int						_userLimit;
		std::map<Client, ClientStatus>	_memberList;//client 0 = not connected/ client 1 = connected/ client 2 = invited / client 3 = op / client 4 = ban
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
		int		getStatusClient(Client &client);
		void	setStatusClient(Client &client, int status);
		std::string		setName();
};

#endif
