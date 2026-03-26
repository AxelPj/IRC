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

void    Server::createChannel(const std::string &channelName, Client *client)
{
    Channel *newChannel = new Channel();
    cmdJoin(newChannel, client, 1);
    this->_listChannel[channelName] = newChannel;
}

void Server::addMode(Channel *channel, char mode, const std::string &param)
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
    {
        Client *target = getClientByName(param);
        if (target)
            channel->setStatusClient(*target, OP);
    }
}

void Server::removeMode(Channel *channel, char mode, const std::string &param)
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
    {
        Client *target = getClientByName(param);
        if (target)
            channel->setStatusClient(*target, CONNECTED);
    }
}