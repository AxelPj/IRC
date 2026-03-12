#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <list>
#include <map>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <vector>
#include <unistd.h>
#include <netinet/in.h>
#include <poll.h>
#include "Client.hpp"

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

class Client;

class Server
{
    private:
        Server();
        int                     _port;
        int                     _sockfd;
        int                     _signal;
        std::map<int, Client>   _listClient;
        std::vector<pollfd>     _socketIrc;
        std::string             _password;
        void quit();
        char    _buffer[1024];
        /*         void ping(Client user);
        */        
       public:
        Server(int port, std::string password);
        ~Server();
        void    init();
        void    run();
        Client  acceptClient(sockaddr_in *addr, pollfd *newSocketclient);
        bool    recvClient(pollfd socketclient);
        void    addToChannel();
        void    addChannel(std::string channel);
        void    removeChannel(std::string channel);
	    void	removeClient(int fdClient, int i);
        void    sendMessage(char *msg, int size, int socket);         
        // getters
        int     getPort() const;
        int     getSockfd() const;
        char    *getBuffer();
        pollfd  getpollfd(int i) const;
};

#endif
