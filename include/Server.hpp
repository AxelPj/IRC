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
#include <cstring>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

class Client;

class Server
{
    
    private:
        Server();
        int                             _port;
        int                             _sockfd;
        int                             _signal;
        std::map<int, Client>           _listClient;
        std::vector<pollfd>             _socketIrc;
        std::string                     _password;
        //std::map<std::string, Channel>   _listChannel;
        void quit();
        char             _buffer[1024];
        /*         void ping(Client user);
        */        

    public:
        Server(int port, std::string password);
        ~Server();
        void    init();
        void    run();
        Client  acceptClient(sockaddr_in *addr, pollfd *newSocketclient);
        bool    recvClient(pollfd socketclient, Client &client);
        void    addToChannel();
        void    addChannel(std::string channel);
        void    removeChannel(std::string channel);
	    void	removeClient(int fdClient, int i);
        void    sendMsg(std::string msg, int socket);

        
        // getters
        int     getPort() const;
        int     getSockfd() const;
        char    *getBuffer();
        pollfd  getpollfd(int i) const;
        
        //Parser general
        void                        processParser(Client &client);
        std::vector<std::string>    tokenParser(std::string buffer);
        int                         choiceParser(std::vector<std::string> tokens);

        //Parser commands
        int parserCmdNick(std::vector<std::string> token);
        int parserCmdJoin(std::vector<std::string> token);
        int parserCmdPart(std::vector<std::string> token);
        int parserCmdPrivMsg(std::vector<std::string> token);
        int parserCmdKick(std::vector<std::string> token);
        int parserCmdInvite(std::vector<std::string> token);
        int parserCmdTopic(std::vector<std::string> token);
        int parserCmdMode(std::vector<std::string> token);
        int parserCmdPing(std::vector<std::string> token);
        int parserCmdPong(std::vector<std::string> token);
		int parserCmdQuit(std::vector<std::string> token);
		int parserCmdReconnect(std::vector<std::string> token);
		
};

#endif
