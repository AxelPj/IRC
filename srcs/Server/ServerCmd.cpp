#include "Server.hpp"

int Server::cmdNick(Client &client, const std::vector<std::string> &token)
{
    client.setNick(token[1]);
    return (0);
}

int Server::cmdPart(Client &client, const std::vector<std::string> &token, bool reason)
{
    _listChannel[token[1]]->setStatusClient(client, false);
    if (reason == true) 
    {
        sendMsg("You have left the channel" + token[1], client.getFd());
        sendMsg("(", client.getFd());
        for (size_t i = 2; i < token.size(); i++)
        {
            sendMsg(token[i], client.getFd());
        }
        sendMsg(")", client.getFd());
        return (0);
    }
    else {
        sendMsg("You have left the channel" + token[1] + "(Leaving)", client.getFd());
        return (0);
    }
    return (1);
}

int Server::cmdJoin(Client &client, Channel &channel, bool setOps)
{
    if (setOps == true)
        channel.setStatusClient(client, 3);
    else if (setOps == false)
        channel.setStatusClient(client, 1);
    sendMsg("You have joined the channel" + channel.setName(), client.getFd());
    return (0);
}

int Server::cmdKick(Client &client, Channel &channel)
{
    channel.setStatusClient(client, 4);
    sendMsg("You have been kicked from the channel " + channel.setName() + " ", client.getFd());
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

/* int Server::cmdInvite(Client &client, const std::vector<std::string> &token)
{
    if (listMember(token[1]) == false)
    {
        return (-1);
    }
    sendMsg("You've invited " + token[1] + " to the channel " + token[2], client.getFd());
    sendMsg(client.getNick() + "has invited " + token[1] + " to the channel " + token[2], client.getFd());
    sendMsg("You have been invited to the channel " + token[2] + " by " + client.getNick(), _listChannel[token[2]]->getClient(token[1])->getFd());
    return (0);
} */

/*void   Server::removeClient(int fd)
{
}*/
