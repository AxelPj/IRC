#include "Server.hpp"

int Server::cmdNick(Client &client, std::vector<std::string> token)
{
    client.setNick(token[1]);
    return (0);
}

int Server::cmdPart(std::vector<std::string> token, Client &client, bool reason)
{
    //TO DO : _listChannel[token[1]]->removeClient(client);
    if (reason == true) {
        sendMsg("You have left the channel" + token[1], client.getfd());
        sendMsg("(", client.getfd());
        for (size_t i = 2; i < token.size(); i++)
        {
            sendMsg(token[i], client.getfd());
        }
        sendMsg(")", client.getfd());
        return (0);
    }
    else {
        sendMsg("You have left the channel" + token[1] + "(Leaving)", client.getfd());
        return (0);
    }
    return (1);
}



/*void   Server::removeClient(int fd)
{
}*/

void    Server::createChannel(std::string channelName)
{
    Channel *newChannel = new Channel();
    this->_listChannel[channelName] = newChannel;
}