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
#include "Channel.hpp"
#include <cstring>
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

class Client;
class channel;

class Server
{
    private:
        Server();
        int                             _port;
        std::string                     _password;
        std::vector<pollfd>             _socketIrc;
        int                             _sockfd;
        int                             _signal;
        std::map<int, Client*>          _listClient;
        std::map<std::string, Channel*> _listChannel;
        char                            _buffer[1024];

    public:
        Server(int port, const std::string &password);
        ~Server();

        void                        init();
        void                        run();
        Client*                     acceptClient(sockaddr_in *addr, pollfd *newSocketclient);
        bool                        recvClient(const pollfd &socketclient, Client &client);
        void                        addToChannel();
        void                        addChannel(const std::string &channel);
        void                        removeChannel(const std::string &channel);
        void                        removeClient(int fdClient, int i);
        bool                        checkChannelExist(const std::string &channelName);
        bool                        checkUserExist(const std::string &userName);

        // getters
        int                         getPort() const;
        int                         getSockfd() const;
        char                        *getBuffer();
        pollfd                      getpollfd(int i) const;
        Client&                     getClient(const std::string &nameClient);
        Channel&                    getChannel(const std::string &name);

        // Parser general
        void                        processParser(Client &client);
        std::vector<std::string>    tokenSpace(const std::string &buffer);
        std::vector<std::string>    tokenComma(const std::string &buffer);
        int                         choiceParser(const std::vector<std::string> &tokens);

        // Parser commands
        int                         parserCmdNick(const std::vector<std::string> &token) const;
        int                         parserCmdJoin(const std::vector<std::string> &tokens, Client &client);
        int                         parserCmdKick(const std::vector<std::string> &tokens, Client &client);
        int                         parserCmdJoinMulti(const std::vector<std::string> &tokens, Client &client);
        int                         parserCmdPart(const std::vector<std::string> &tokens);
        int                         parserCmdPartMulti(const std::vector<std::string> &tokens, Client &client);
        int                         parserCmdPrivMsg(const std::vector<std::string> &token);
        int                         parserCmdPrivMsgMulti(const std::vector<std::string> &tokens, Client &client);
        int                         parserCmdInvite(const std::vector<std::string> &tokens, Client &client);
        int                         parserCmdTopic(const std::vector<std::string> &tokens, const Client& client);
        int                         parserCmdMode(const std::vector<std::string> &tokens, Client &client);
        int                         parserCmdQuit(const std::vector<std::string> &tokens);
        int                         parserCmdReconnect(const std::vector<std::string> &tokens);

        // commands
        int                         cmdJoin(const Client &client, const std::string &channel, bool setOps);
        int                         cmdPart(Client &client, const std::vector<std::string> &token, bool reason);
        int                         cmdNick(Client &client, const std::vector<std::string> &token);
        int                         cmdPrivMsg(Client &client, const std::vector<std::string> &token);
        int                         cmdKick(const std::vector<std::string> token, Client &client, Channel &channel, bool reason);
        int                         cmdInvite(Client &client, const std::vector<std::string> &token);
        int                         cmdTopic(const std::vector<std::string> &token);
        int                         cmdMode(Client &client, const std::vector<std::string> &token);
        void                        cmdPing(Client &client, const std::vector<std::string> &token);
        void                        cmdPong(Client &client, const std::vector<std::string> &token);
        int                         cmdQuit(Client& client, const std::string& reason);
        int                         cmdReconnect(Client &client, const std::vector<std::string> &token);

        // others
        void                        sendMsg(const std::string &msg, int socket);
        void                        sendMsgChan(const std::string& msg, Channel& channel, int senderFd);
        void                        createChannel(const std::string &channelName, const Client *client);
        void                        addMode(Channel *channel, char mode, const std::string &param, Client& client);
        void                        removeMode(Channel *channel, char mode, Client& client);
        void                        removeClient(Client &client);
};

#endif