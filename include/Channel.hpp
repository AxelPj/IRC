#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"
#include "Client.hpp"
#include <string>
#include <map>

class Server;
class Client;

class Channel
{
	private:
		const Server		_server;
		const std::string	_name;
		std::string			_topic;
		int					_modes; //bitmask in order p/s/i/t/n/m
		int					_userLimit;
		std::map<Client, bool>	_memberList;
		//todo banlist container
		
		void	join(Client user);
		void	setMode(bool isRemoved, int mask);
		//void	setUserMode(Client user, bool isRemoved, int mask);
		void	invite(Client user);

	public:
		Channel();
		~Channel();
		Channel(Server serv, Client cli, std::string name);

		std::string& getName();
		std::string& getTopic();
		void setTopic(std::string topic);
};

#endif
