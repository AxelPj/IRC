/*Classe Client (fd, nick, user, état auth), 
Authentification mot de passe, Gestion pseudo, 
Enregistrement utilisateur, Déconnexion propre, 
Messages directs, Keepalive*/

// PASS(?), USER(?), 
// NICK = change Nickname,
// QUIT = disconnect from the server,
// RECONNECT = attempt to reconnect to the server,
// PRIVMSG = send a private message to a user or channel,
// NOTICE = send a notice to a user or channel,
// PING, PONG = keep-alive messages (not mandatory).

#pragma once

#include <iostream>
#include "Server.hpp"

class Channel;

class Client {
	public :
		Client(const sockaddr_in addrClient, const pollfd Socketclient, const std::string &pass);
		Client& operator=(const Client& other);
		~Client();

		//Getters//
		std::string getPassword()const;
		std::string getAddress()const;
		std::string getNick()const;
		std::string getUser()const;
		std::string getRealName()const;
		std::string	getBuffer()const;
		int			getFd() const;
		bool		isAway() const;
		bool		isAuth() const;
		bool		getRegistered() const;
		//Setters//
		void		setNick(const std::string &nick);
		void		setAddBuffer(char *msg);
		void		setRemoveBuffer();
		void		setBuffer(const std::string &buffer);
		void		setUser(const std::string& user);
		void		setRealName(const std::string& name);
		void		setAway(bool away);
		void		setAuth(bool auth);
		void		setRegistered(bool registered);
		//Utils
		bool		canSee(const Client& client);
		bool		canSee(const Channel& channel);
		char		getAwayStatus();

	private :
		Client();
		int _fd;
		std::string _password;
		std::string _address;
		std::string _nick;
		std::string _username;
		std::string _realName;
		std::string _buffer;
		bool		_authentified;
		bool		_registered;
		bool		_isAway;
};
