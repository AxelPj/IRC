#include "Server.hpp"
#include "IRCNumerics.hpp"

int Server::cmdNick(Client &client, const std::vector<std::string> &token)
{
    std::string oldNick = client.getNick();
    std::string newNick = token[1];
    if (newNick.size() > 30)
        newNick = newNick.substr(0, 30);
    std::string host = client.getAddress();
    client.setNick(newNick);
    sendMsg(MSG_NICK(oldNick, "realuser", host, newNick), client);
    if (client.getUser().empty() == false && client.getRegistered() == false)
    {
        client.setRegistered(true);
        sendMsg(RPL_WELCOME("server", client.getNick(), client.getUser(), client.getAddress()), client);
        sendMsg(RPL_YOURHOST("server", client.getNick(), "1.0"), client);
        sendMsg(RPL_CREATED("server", client.getNick(), "28 Mar 2026"), client);
    }
    return (0);
}

int Server::cmdUser(Client &client, const std::vector<std::string> &token)
{
    client.setUser(token[1]);
    if (client.getNick().empty() == false && client.getRegistered() == false)
    {
        client.setRegistered(true);
        sendMsg(RPL_WELCOME("server", client.getNick(), client.getUser(), client.getAddress()), client);
        sendMsg(RPL_YOURHOST("server", client.getNick(), "1.0"), client);
        sendMsg(RPL_CREATED("server", client.getNick(), "28 Mar 2026"), client);
    }
    //sendMsg(RPL_MYINFO("server", client.getNick(), "1.0", "", "itkol"), client.getFd(), client.getAddress());
    //sendMsg(ERR_NOMOTD("server", client.getNick()), client.getFd(), client.getAddress());
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
    sendMsg(MSG_PART(client.getNick(), "realuser", client.getAddress(), token[1], msg), client);
    sendMsgChan(MSG_PART(client.getNick(), "realuser", client.getAddress(), token[1], msg), getChannel(token[1]), client.getFd());
    return (0);
}

int Server::cmdPartMulti(Client &client, const std::vector<std::string> &chan, const std::string &reason, bool all)
{
	if (all == true)
	{
		for (std::map<std::string, Channel*>::const_iterator i = this->_listChannel.begin(); i != _listChannel.end(); i++)
		{
			if (i->second->isMember(client))
			{
                sendMsg(MSG_PART(client.getNick(), "realuser", client.getAddress(), i->first, ""), client);
				sendMsgChan(MSG_PART(client.getNick(), "realuser", client.getAddress(), i->first, ""), *i->second, client.getFd());
				i->second->removeMember(client);    
			}
		}
		return 0 ;
	}
    for (size_t i = 0; i < chan.size(); i++)
    {
        this->_listChannel[chan[i]]->removeMember(client);
        sendMsg(MSG_PART(client.getNick(), "realuser", client.getAddress(), chan[i], reason), client);
        sendMsgChan(MSG_PART(client.getNick(), "realuser", client.getAddress(), chan[i], reason), getChannel(chan[i]), client.getFd());
    }
    return (0);
}

int Server::cmdJoin(const Client &client, const std::string &channel, bool setOps)
{
    std::string host = client.getAddress();
    if (setOps == true)
    {
        createChannel(channel, &client);
        this->_listChannel[channel]->setStatusClient(client, OP);
    }
    else if (setOps == false)
        this->_listChannel[channel]->setStatusClient(client, CONNECTED);
    sendMsg(MSG_JOIN(client.getNick(), "realuser", host, channel), client);
	sendMsgChan(MSG_JOIN(client.getNick(), "realuser", host, channel), getChannel(channel), client.getFd());
	Channel chan = getChannel(channel);
	if (chan.getTopic() != "")
		sendMsg(RPL_TOPIC("server", client.getNick(), channel, chan.getTopic()), client);
	else
		sendMsg(RPL_NOTOPIC("server", client.getNick(), channel), client);
	sendMsg(chan.namesReply(client), client);
	sendMsg(RPL_ENDOFNAMES("server", client.getNick(), channel), client);
    return (0);
}

int Server::cmdKick(const std::vector<std::string> token, Client &client, Channel &channel, bool reason)
{
    std::string host = client.getAddress();
    std::string msg = "";
    if(reason == true)
    {
        for (size_t i = 2; i < token.size(); i++)
            msg += token[i];
    }
    sendMsg(MSG_KICK(token[0], "realuser", host, channel.getName(), token[2], msg), client);
    sendMsgChan(MSG_KICK(token[0], "realuser", host, channel.getName(), token[2], msg), channel, INVALID_SOCKET);
    channel.removeMember(client);
    return (0);
}

void Server::cmdPong(Client &client, const std::vector<std::string> &token)
{
    sendMsg(std::string("PONG :") + token[1] + "\r\n", client);
}

void Server::cmdPing(Client &client, const std::vector<std::string> &token)
{
    sendMsg(std::string("PING :") + token[1] + "\r\n", client);
}

int Server::cmdTopic(const std::vector<std::string> &tokens, const Client& client)
{
    Channel &chan = getChannel(tokens[1]);
    chan.setTopic(tokens[2]);
    sendMsg(MSG_TOPIC(client.getNick(), "realuser", client.getAddress(), tokens[1], tokens[2]), client);
	sendMsgChan(MSG_TOPIC(client.getNick(), "realuser", client.getAddress(), tokens[1], tokens[2]), getChannel(tokens[1]), client.getFd());
    return(0);
}

int Server::cmdInvite(Client &client, const std::vector<std::string> &token)
{
	if (token.size() == 1)
	{
		for (std::map<std::string, Channel*>::const_iterator i = _listChannel.begin(); i != _listChannel.end(); i++)
		{
			if (i->second->isInvited(client))
				sendMsg(RPL_INVITELIST("server", client.getNick(), i->first), client);
		}
		sendMsg(RPL_ENDOFINVITELIST("server", client.getNick()), client);
		return 0;
	}
    Client invitedClient = getClient(token[1]);
    Channel &chan = getChannel(token[2]);
	chan.invite(invitedClient);
    sendMsg(RPL_INVITING("server", client.getNick(), token[1], token[2]), client);
	sendMsg(MSG_INVITE(client.getNick(), "realuser", client.getAddress(), token[1], token[2]), invitedClient);
    return (0);
}

int Server::cmdQuit(Client& client, const std::string& reason)
{
    std::string host = client.getAddress();
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
