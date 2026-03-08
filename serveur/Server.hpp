#include <iostream>
#include <list>
#include <map>
#include <string>
#include "Client.hpp"

class Serveur
{
private:
    std::list<std::string> _channels;
    int _socket;
    std::map<int Id, std::string username> Users; 
    int port;
    std::string _password;
    void ping(Client user);
    void quit();

    public:
    Serveur(int port, std::string password);
    ~Serveur();
    void    addToChannel();
    void    addChannel(std::string channel);
    void    removeChannel(std::string channel);
    void    sendMessage(std::string message, Client user);
};


