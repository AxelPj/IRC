#include "Server.hpp"
#include "IRCNumerics.hpp"

void Server::cmdPass(Client &client, const std::string &pass)
{
	client.setAuth(pass == this->_password);
}

void Server::cmdNick(Client &client, const std::vector<std::string> &token)
{
    std::string oldNick = client.getNick();
    std::string newNick = token[1];
    if (newNick.size() > 30)
        newNick = newNick.substr(0, 30);
    std::string host = client.getAddress();
    client.setNick(newNick);
    broadcastMsg(MSG_NICK(oldNick, client.getUser(), host, newNick));
    if (client.getUser().empty() == false && client.getRegistered() == false)
    {
        client.setRegistered(true);
        sendMsg(RPL_WELCOME(SERVER_NAME, client.getNick(), client.getUser(), client.getAddress()), client);
        sendMsg(RPL_YOURHOST(SERVER_NAME, client.getNick(), "1.0"), client);
        sendMsg(RPL_CREATED(SERVER_NAME, client.getNick(), "28 Mar 2026"), client);
    }
    return ;
}

void Server::cmdUser(Client &client, const std::vector<std::string> &token)
{
    client.setUser(token[1]);
	client.setRealName(token[4]);
    if (client.getNick().empty() == false && client.getRegistered() == false)
    {
        client.setRegistered(true);
        sendMsg(RPL_WELCOME(SERVER_NAME, client.getNick(), client.getUser(), client.getAddress()), client);
        sendMsg(RPL_YOURHOST(SERVER_NAME, client.getNick(), "1.0"), client);
        sendMsg(RPL_CREATED(SERVER_NAME, client.getNick(), "28 Mar 2026"), client);
    }
    return ;
}

void Server::cmdPart(Client &client, const std::vector<std::string> &token, bool reason)
{
    _listChannel[token[1]]->removeMember(client);
    std::string msg = "";
    if (reason == true)
    {
        for (size_t i = 2; i < token.size(); i++)
            msg += token[i];
    }
    sendMsg(MSG_PART(client.getNick(), client.getUser(), client.getAddress(), token[1], msg), client);
    sendMsgChan(MSG_PART(client.getNick(), client.getUser(), client.getAddress(), token[1], msg), getChannel(token[1]), client.getFd());
    if (this->_listChannel[token[1]]->isEmpty())
    {
        delete _listChannel[token[1]];
        this->_listChannel.erase(token[1]);
    }
    return ;
}

void Server::cmdPartMulti(Client &client, const std::vector<std::string> &chan, const std::string &reason, bool all)
{
	if (all == true)
	{
		for (std::map<std::string, Channel*>::iterator i = this->_listChannel.begin(); i != _listChannel.end();)
		{
			if (i->second->isMember(client))
			{
                sendMsg(MSG_PART(client.getNick(), client.getUser(), client.getAddress(), i->first, ""), client);
				sendMsgChan(MSG_PART(client.getNick(), client.getUser(), client.getAddress(), i->first, ""), *i->second, client.getFd());
				i->second->removeMember(client);
                if (i->second->isEmpty())
                {
                    delete i->second;
                    _listChannel.erase(i++);
                    continue ;
                }
				i++;
			}
			else
				i++;
		}
		return ;
	}
    for (size_t i = 0; i < chan.size(); i++)
    {
        this->_listChannel[chan[i]]->removeMember(client);
        if (_listChannel[chan[i]]->isEmpty())
        {
            delete _listChannel[chan[i]];
            _listChannel.erase(chan[i]);
        }
        else
        {
            sendMsg(MSG_PART(client.getNick(), client.getUser(), client.getAddress(), chan[i], reason), client);
            sendMsgChan(MSG_PART(client.getNick(), client.getUser(), client.getAddress(), chan[i], reason), getChannel(chan[i]), client.getFd());
        }
    }
    return ;
}

void Server::cmdJoin(const Client &client, const std::string &channel, bool setOps)
{
    std::string host = client.getAddress();
    if (setOps == true)
    {
        createChannel(channel, &client);
        this->_listChannel[channel]->setStatusClient(client, OP);
    }
    else if (setOps == false)
        this->_listChannel[channel]->setStatusClient(client, CONNECTED);
    sendMsg(MSG_JOIN(client.getNick(), client.getUser(), host, channel), client);
	sendMsgChan(MSG_JOIN(client.getNick(), client.getUser(), host, channel), getChannel(channel), client.getFd());
	Channel chan = getChannel(channel);
	if (chan.getTopic() != "")
		sendMsg(RPL_TOPIC(SERVER_NAME, client.getNick(), channel, chan.getTopic()), client);
	else
		sendMsg(RPL_NOTOPIC(SERVER_NAME, client.getNick(), channel), client);
	sendMsg(chan.namesReply(client), client);
	sendMsg(RPL_ENDOFNAMES(SERVER_NAME, client.getNick(), channel), client);
    return ;
}

void Server::cmdModeChannel( Client &client, const std::vector<std::string> &tokens)
{
    Channel &channel = getChannel(tokens[1]);
	std::string modes = tokens[2];
	std::string param;

	char sign = modes[0];
    size_t paramIndex = 3;
    for (size_t i = 1; i < modes.size(); i++)
    {
        if (modes[i] == '+' || modes[i] == '-')
        {
            sign = modes[i];
            continue;
        }
        char mode = modes[i];
        if ((mode == 'o') || (sign == '+' && (mode == 'k' || mode == 'l')))
			param = tokens[paramIndex++];
		else
			param = "";
        if (sign == '+')
            addMode(channel, mode, param);
        else
            removeMode(channel, mode, param);
			sendMsg(MSG_MODE(client.getNick(), client.getUser(), client.getAddress(), channel.getName(), sign + mode, param), client);
			sendMsgChan(MSG_MODE(client.getNick(), client.getUser(), client.getAddress(), channel.getName(), sign + mode, param), channel, client.getFd());
    }
}

void Server::cmdKick(Client &kicker, const std::vector<std::string> token, Client &client, Channel &channel, bool reason)
{
    std::string host = kicker.getAddress();
    std::string msg = "";
    if(reason == true)
    {
        for (size_t i = 2; i < token.size(); i++)
            msg += token[i];
    }
    sendMsg(MSG_KICK(kicker.getNick(), kicker.getUser(), host, channel.getName(), token[1], msg), client);
    sendMsgChan(MSG_KICK(kicker.getNick(), kicker.getUser(), host, channel.getName(), token[1], msg), channel, INVALID_SOCKET);
    channel.removeMember(client);
}

void Server::cmdPong(Client &client, const std::vector<std::string> &token)
{
    sendMsg(std::string("PONG :") + token[1] + "\r\n", client);
}

void Server::cmdPing(Client &client, const std::vector<std::string> &token)
{
    sendMsg(std::string("PING :") + token[1] + "\r\n", client);
}

void Server::cmdTopic(const std::vector<std::string> &tokens, const Client& client)
{
    Channel &chan = getChannel(tokens[1]);
    chan.setTopic(tokens[2]);
    sendMsg(MSG_TOPIC(client.getNick(), client.getUser(), client.getAddress(), tokens[1], tokens[2]), client);
	sendMsgChan(MSG_TOPIC(client.getNick(), client.getUser(), client.getAddress(), tokens[1], tokens[2]), getChannel(tokens[1]), client.getFd());
    return ;
}

void Server::cmdInvite(Client &client, const std::vector<std::string> &token)
{
	if (token.size() == 1)
	{
		for (std::map<std::string, Channel*>::const_iterator i = _listChannel.begin(); i != _listChannel.end(); i++)
		{
			if (i->second->isInvited(client))
				sendMsg(RPL_INVITELIST(SERVER_NAME, client.getNick(), i->first), client);
		}
		sendMsg(RPL_ENDOFINVITELIST(SERVER_NAME, client.getNick()), client);
		return ;
	}
    Client invitedClient = getClient(token[1]);
    Channel &chan = getChannel(token[2]);
	chan.invite(invitedClient);
    sendMsg(RPL_INVITING(SERVER_NAME, client.getNick(), token[1], token[2]), client);
	sendMsg(MSG_INVITE(client.getNick(), client.getUser(), client.getAddress(), token[1], token[2]), invitedClient);
    return ;
}

void Server::cmdQuit(Client& client, const std::string& reason)
{
    std::string host = client.getAddress();
    std::string reasonStr = reason.empty() ? "Quit" : reason;
    for (std::map<std::string, Channel*>::iterator it = this->_listChannel.begin(); it != this->_listChannel.end();)
    {
        if (it->second->isMember(client))
        {
            sendMsgChan(MSG_QUIT(client.getNick(), client.getUser(), host, reasonStr), *it->second, client.getFd());
            it->second->removeMember(client);
            if (it->second->isEmpty())
            {
                delete it->second;
                _listChannel.erase(it++);
                continue ;
            }
        }
        it++;
    }
    return ;
}

void Server::cmdWhoUser(Client &client, const std::string& nick)
{
	Client target = getClient(nick);
	if (client.canSee(target))
		sendMsg(RPL_WHOREPLY(SERVER_NAME, client.getNick(), "*", target.getUser(), target.getAddress(), nick, target.getAwayStatus(), target.getRealName()), client);
	sendMsg(RPL_ENDOFWHO(SERVER_NAME, client.getNick(), nick), client);
}

void Server::cmdWhoChannel(Client &client, const std::string& chanName)
{
	Channel &chan = getChannel(chanName);
	if (client.canSee(chan))
	{
		for (std::map<const Client*, ClientStatus>::iterator it = chan.getMemberList().begin(); it != chan.getMemberList().end(); it++)
		{
			if (it->second != INVITED && client.canSee(*it->first))
			{
				Client target = *it->first;
				sendMsg(RPL_WHOREPLY(SERVER_NAME, client.getNick(), chanName, target.getUser(), target.getAddress(), target.getNick(), target.getAwayStatus(), target.getRealName()), client);
			}
		}
	}
	sendMsg(RPL_ENDOFWHO(SERVER_NAME, client.getNick(), chanName), client);
}
