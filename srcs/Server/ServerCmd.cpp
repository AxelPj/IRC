#include "Server.hpp"
#include "IRCNumerics.hpp"

int Server::cmdNick(Client &client, const std::vector<std::string> &token)
{
    client.setNick(token[1]);
    if (client.getUser().empty() == false && client.getRegistered() == false)
    {
        client.setRegistered(true);
        sendMsg(RPL_WELCOME("server", client.getNick(), client.getUser(), client.getAdress()), client.getFd(), client.getAdress());
        sendMsg(RPL_YOURHOST("server", client.getNick(), "1.0"), client.getFd(), client.getAdress());
        sendMsg(RPL_CREATED("server", client.getNick(), "28 Mar 2026"), client.getFd(), client.getAdress());
        sendMsg(RPL_MYINFO("server", client.getNick(), "1.0", "", "itkol"), client.getFd(), client.getAdress());
        sendMsg(ERR_NOMOTD("server", client.getNick()), client.getFd(), client.getAdress());
    }
    return (0);
}

int Server::cmdUser(Client &client, const std::vector<std::string> &token)
{
    client.setUser(token[1]);
    if (client.getNick().empty() == false && client.getRegistered() == false)
    {
        client.setRegistered(true);
        sendMsg(RPL_WELCOME("server", client.getNick(), client.getUser(), client.getAdress()), client.getFd(), client.getAdress());
        sendMsg(RPL_YOURHOST("server", client.getNick(), "1.0"), client.getFd(), client.getAdress());
        sendMsg(RPL_CREATED("server", client.getNick(), "28 Mar 2026"), client.getFd(), client.getAdress());
        sendMsg(RPL_MYINFO("server", client.getNick(), "1.0", "", "itkol"), client.getFd(), client.getAdress());
        sendMsg(ERR_NOMOTD("server", client.getNick()), client.getFd(), client.getAdress());
    }
    return (0);
}

int Server::cmdPart(Client &client, const std::vector<std::string> &token, bool reason)
{
    _listChannel[token[1]]->removeMember(client);
    std::string msg = "";
    if (reason == true)
    {
        for (size_t i = 2; i < token.size(); i++)
            msg += token[i];
    }
    sendMsg(MSG_PART(client.getNick(), "realuser", client.getAdress(), token[1], msg), client.getFd(), client.getAdress());
    sendMsgChan(MSG_PART(client.getNick(), "realuser", client.getAdress(), token[1], msg), getChannel(token[1]), client.getFd());
    return (0);
}

int Server::cmdJoin(const Client &client, const std::string &channel, bool setOps)
{
    std::string host = client.getAdress();
    if (setOps == true)
    {
        createChannel(channel, &client);
        this->_listChannel[channel]->setStatusClient(client, OP);
    }
    else if (setOps == false)
    {
        this->_listChannel[channel]->setStatusClient(client, CONNECTED);
    }
	sendMsgChan(MSG_JOIN(client.getNick(), "realuser", host, channel), getChannel(channel), client.getFd());
	// TODO VERIFY THESE NUMERICS
	sendMsg(RPL_TOPIC(client.getNick(), "realuser", host, channel), client.getFd(), host);
	Channel chan = getChannel(channel);
    for (std::map<const Client*, ClientStatus>::iterator it = chan.getMemberList().begin(); it != chan.getMemberList().end(); it++)
		sendMsg(RPL_NAMREPLY("server", client.getNick(), channel, it->first->getNick()), client.getFd(), client.getAdress());
	//TODO sendMsg(RPL_ENDOFNAMES);
    return (0);
}

int Server::cmdKick(const std::vector<std::string> token, Client &client, Channel &channel, bool reason)
{
    std::string host = client.getAdress();
    std::string msg = "";
    if(reason == true)
    {
        for (size_t i = 2; i < token.size(); i++)
            msg += token[i];
    }
    sendMsg(MSG_KICK(token[0], "realuser", host, channel.getName(), token[2], msg), client.getFd(), host);
    sendMsgChan(MSG_KICK(token[0], "realuser", host, channel.getName(), token[2], msg), channel, INVALID_SOCKET);
    channel.removeMember(client);
    return (0);
}

void Server::cmdPong(Client &client, const std::vector<std::string> &token)
{
    sendMsg(std::string("PONG :") + token[1] + "\r\n", client.getFd(), client.getAdress());
}

void Server::cmdPing(Client &client, const std::vector<std::string> &token)
{
    sendMsg(std::string("PING :") + token[1] + "\r\n", client.getFd(), client.getAdress());
}

int Server::cmdTopic(const std::vector<std::string> &tokens, const Client& client)
{
    Channel &chan = getChannel(tokens[1]);
    chan.setTopic(tokens[2]);
    std::string host = client.getAdress();
	sendMsgChan(MSG_TOPIC(client.getNick(), "realuser", host, tokens[1], tokens[2]), getChannel(tokens[1]), client.getFd());
    return(0);
}

int Server::cmdInvite(Client &client, const std::vector<std::string> &token)
{
	if (token.size() == 1)
	{
		std::string invList = "";
		for (std::map<std::string, Channel*>::const_iterator i = _listChannel.begin(); i != _listChannel.end(); i++)
		{
			if (i->second->isInvited(client))
				sendMsg(RPL_INVITELIST("server", client.getNick(), i->first), client.getFd(), client.getAdress());
		}
		sendMsg(RPL_ENDOFINVITELIST("server", client.getNick()), client.getFd(), client.getAdress());
		return 0;
	}
    Client *invitedClient = &getClient(token[1]);
    std::string host = client.getAdress();
    sendMsg(RPL_INVITING("server", client.getNick(), token[1], token[2]), client.getFd(), host);
    sendMsg(RPL_INVITING("server", invitedClient->getNick(), token[1], token[2]), invitedClient->getFd(), invitedClient->getAdress());
    sendMsgChan(std::string(":") + client.getNick() + "!" + "realuser" + "@" + host + " INVITE " + token[1] + " :" + token[2] + "\r\n", getChannel(token[2]), client.getFd());
    return (0);
}

int Server::cmdQuit(Client& client, const std::string& reason)
{
    std::string host = client.getAdress();
    std::string reasonStr = reason.empty() ? "Quit" : reason;
    for (std::map<std::string, Channel*>::iterator it = this->_listChannel.begin(); it != this->_listChannel.end(); it++)
    {
        if (it->second->isMember(client))
        {
            sendMsgChan(MSG_QUIT(client.getNick(), "realuser", host, reasonStr), *it->second, client.getFd());
            it->second->removeMember(client);
        }
    }
    return (0);
}
