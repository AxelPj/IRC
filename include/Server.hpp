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
enum e_cmd
{
    NICK,
    JOIN,
    PART,
    PRIVMSG,
    KICK,
    INVITE,
    TOPIC,
    MODE,
    QUIT,
    PING,
    PONG,
    UNKNOWN
};

enum e_mode
{
    
}
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

        //Parser
        void                processParser(Client &client);
        std::vector<std::string>  tokenParser(std::string buffer);
        e_cmd               choiceParser(std::vector<char*> buffer);
        // getters
        int     getPort() const;
        int     getSockfd() const;
        char    *getBuffer();
        pollfd  getpollfd(int i) const;
};

#endif
