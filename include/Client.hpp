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

class Client {
	public :
		Client(const sockaddr_in addrClient, const pollfd Socketclient);
		Client& operator=(const Client& other);
		~Client();

		//Getters//
		std::string getPassword();
		std::string getAdress();
		std::string getNick();
		std::string getSecond();
		std::string getThird();
		std::string getUser();
		std::string	getBuffer();

		//Setters//
		void setAddBuffer(char *msg);
		void setRemoveBuffer();
		//Commands//
		int cmdNick(const std::string& newNick, const Client& client);
		int cmdQuit(const Client& client);
		int cmdReconnect(const Client& client);
		
	private :
		Client();
		bool _ops;
		int _fd;
		std::string _password;
		std::string _adress;
		std::string _nick;
		std::string _second;
		std::string _third;
		std::string _username;
		std::string _buffer;
};