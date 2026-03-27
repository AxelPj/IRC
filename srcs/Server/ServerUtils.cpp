#include "Server.hpp"

bool Server::checkChannelExist(const std::string &channelName)
{
    return (this->_listChannel.find(channelName) != this->_listChannel.end());
}

bool Server::checkUserExist(const std::string &userName)
{
    for (std::map<int, Client*>::iterator it = this->_listClient.begin(); it != this->_listClient.end(); it++)
    {
        if (it->second->getUser() == userName)
            return (true);
    }
    return (false);
}

std::vector<std::string> Server::tokenComma(const std::string &buffer)
{
    char *bufCpy = strdup(buffer.c_str());
    std::vector<std::string> tokens;
    char *token;

    token = strtok(bufCpy, " ");
    if (token == nullptr)
    {
        free(bufCpy);
        return (tokens);
    }
    tokens.push_back(token);
    while ((token = strtok(nullptr, ",")) != nullptr)
    {
        if (token[0] == ':')
        {
            std::string tmp = token + 1;
            token = strtok(nullptr, "\r\n");
            if (token != nullptr)
                tmp += token;
            tokens.push_back(tmp);
            free(bufCpy);
            return (tokens);
        }
        else
            tokens.push_back(token);
    }
    free(bufCpy);
    return (tokens);
}

std::vector<std::string> Server::tokenSpace(const std::string &buffer)
{
    char *bufCpy = strdup(buffer.c_str());
    std::vector<std::string> tokens;
    char *token;

    token = strtok(bufCpy, " ");
    if (token == nullptr)
    {
        free(bufCpy);
        return (tokens);
    }
    tokens.push_back(token);
    while ((token = strtok(nullptr, " ")) != nullptr)
    {
        if (token[0] == ':')
        {
            std::string tmp = token + 1;
            token = strtok(nullptr, "\r\n");
            if (token != nullptr)
                tmp += token;
            tokens.push_back(tmp);
            free(bufCpy);
            return (tokens);
        }
        else
            tokens.push_back(token);
    }
    free(bufCpy);
    return (tokens);
}

Channel&     Server::createChannel(const std::string &channelName, const Client *client)
{
    Channel *newChannel = new Channel(*client, channelName);
    this->_listChannel[channelName] = newChannel;
    return (*newChannel);
}

void Server::addMode(Channel *channel, char mode, const std::string &param, Client& client)
{
    if (mode == 'i')
        channel->setInviteOnly(true);
    else if (mode == 't')
        channel->setTopicResctriction(true);
    else if (mode == 'k')
        channel->setPassword(param);
    else if (mode == 'l')
        channel->setUserLimit(atoi(param.c_str()));
    else if (mode == 'o')
        channel->setStatusClient(client, OP);
}

void Server::removeMode(Channel *channel, char mode, Client& client)
{
    if (mode == 'i')
        channel->setInviteOnly(false);
    else if (mode == 't')
        channel->setTopicResctriction(false);
    else if (mode == 'k')
        channel->setPassword("");
    else if (mode == 'l')
        channel->setUserLimit(0);
    else if (mode == 'o')
        channel->setStatusClient(client, CONNECTED);
}

void    Server::sendMsg(const std::string& msg, int socket)
{
    int ret;
    
    const char *buf = msg.c_str();
    while (strlen(buf) != 0)
    {
        ret = send(socket, buf, strlen(buf), 0);
        if (ret == -1)
        {
            std::cerr << "Error: impossible send message to client";
            break ;
        }
        else if (ret == 0)
            break ;
        else
            buf += ret;
    }
}

void Server::sendMsgChan(const std::string& msg, Channel& channel, int senderFd)
{
    std::map<const Client*, ClientStatus>& members = channel.getMemberList();

    for (std::map<const Client*, ClientStatus>::const_iterator i = members.begin();i != members.end();++i)
    {
        const Client* client = i->first;
        ClientStatus status = i->second;

        if (status != CONNECTED && status != OP)
            continue;
        if (client->getFd() == senderFd)
            continue;
        sendMsg(msg, client->getFd());
    }
}