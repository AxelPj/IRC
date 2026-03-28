#include "Server.hpp"

int Server::cmdNick(Client &client, const std::vector<std::string> &token)
{
    client.setNick(token[1]);
    return (0);
}

// TODO format messages in pseudo BNF
int Server::cmdPart(Client &client, const std::vector<std::string> &token, bool reason)
{
    _listChannel[token[1]]->setStatusClient(client, NOT_CONNECTED);
    if (reason == true) 
    {
        std::string msg = "You have left the channel " + token[1] + " (";
        for (size_t i = 2; i < token.size(); i++)
            msg += token[i];
        msg += ")\r\n";
        sendMsg(msg, client.getFd());
        std::string channelMsg = client.getNick() + " has left the channel " + token[1] + " (";
        for (size_t i = 2; i < token.size(); i++)
            channelMsg += token[i];
        channelMsg += ")\r\n";
        sendMsgChan(channelMsg, getChannel(token[1]), client.getFd());
        return (0);
    }
    else 
    {
        sendMsg("You have left the channel " + token[1] + " (Leaving)\r\n", client.getFd());
        sendMsgChan(client.getNick() + " has left the channel " + token[1] + " (Leaving)\r\n", getChannel(token[1]), client.getFd());
        return (0);
    }
}

int Server::cmdJoin(const Client &client, const std::string &channel, bool setOps)
{
    if (setOps == true)
    {
        createChannel(channel, &client);
        this->_listChannel[channel]->setStatusClient(client, OP);
        sendMsg("You have joined the channel " + channel + "\r\n", client.getFd());
    }
    else if (setOps == false)
    {
        this->_listChannel[channel]->setStatusClient(client, CONNECTED);
        sendMsgChan(client.getNick() + " joined the channel " + channel + "\r\n", getChannel(channel), client.getFd());
    }
    return (0);
}

int Server::cmdKick(const std::vector<std::string> token, Client &client, Channel &channel, bool reason)
{   
    if(reason == true)
    {
        std::string msg = "You have been kicked from the channel " + channel.getName() + " (";
        for (size_t i = 2; i < token.size(); i++)
            msg += token[i];
        msg += ")\r\n";
        sendMsg(msg, client.getFd());
    }
    else
        sendMsg("You have been kicked from the channel " + channel.getName() + " " + "(Kicked)\r\n", client.getFd());
    channel.removeMember(client);
    return (0);
}

void Server::cmdPong(Client &client, const std::vector<std::string> &token)
{
    sendMsg("PONG :" + token[1] + "\r\n", client.getFd());
}

void Server::cmdPing(Client &client, const std::vector<std::string> &token)
{
    sendMsg("PING :" + token[1] + "\r\n", client.getFd());
}

int Server::cmdTopic(const std::vector<std::string> &tokens, const Client& client)
{
    Channel &chan = getChannel(tokens[1]);
    chan.setTopic(tokens[2]);
	sendMsgChan("TOPIC " + tokens[1] + " :" + tokens[2] + "\r\n", getChannel(tokens[1]), client.getFd());
    return(0);
}

int Server::cmdInvite(Client &client, const std::vector<std::string> &token)
{
    Client *invitedClient = &getClient(token[1]);
    
    sendMsg("You've invited " + token[1] + " to the channel " + token[2] + "\r\n", client.getFd());
    sendMsg("You have been invited to the channel " + token[2] + " by " + client.getNick() + "\r\n", invitedClient->getFd());
    sendMsgChan(client.getNick() + " has invited " + token[1] + " to the channel " + token[2] + "\r\n", getChannel(token[2]), client.getFd());
    return (0);
}

int Server::cmdQuit(Client& client, const std::string& reason)
{   
    for (std::map<std::string, Channel*>::iterator it = this->_listChannel.begin(); it != this->_listChannel.end(); it++)
    {
        if (it->second->isMember(client))
        {
            sendMsgChan("Quit: " + reason + "\r\n", *it->second, client.getFd());
            it->second->removeMember(client);
        }
    }
    return (0);
}
