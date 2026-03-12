#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"
#include "Client.hpp"
#include <map>

class Server;
class Client;

class Channel
{
	private:
		Server				_server;
		int					_modes;
		int					_userLimit;
		map<Client, int>	_memberList;
		
		void	setMode(bool isRemoved, int mask);
		void	setUserMode(Client user, bool isRemoved, int mask);
		void	removeUser(Client user);
		void	invite(Client user);

	public:
		Channel();
		~Channel();
		Channel(Server serv, Client cli, std::string name);
};

#endif
