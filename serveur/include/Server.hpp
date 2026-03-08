#include <iostream>
#include <list>
#include <map>
#include <string>
#include "Client.hpp"

class Serveur
{
    private:
        int     _port;
        int     _socket;
        std::map<enum idChall, std::string nameChannel> _channels;
        std::map<int idUser, std::string username> _users;
        std::string _password;
        void quit();
        void ping(Client user);
        
    public:
        Serveur(int port, std::string password);
        ~Serveur();
        void    addToChannel();
        void    addChannel(std::string channel);
        void    removeChannel(std::string channel);
        void    sendMessage(std::string message, Client user);
};


