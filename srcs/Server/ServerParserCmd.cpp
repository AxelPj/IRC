#include "Server.hpp"
#include "IRCNumerics.hpp"

int Server::parserCmdUser(const std::vector<std::string> &tokens, const Client &client)
{
    // USER <username> <hostname> <servername> :<realname>
    if (tokens.size() < 5 || tokens[1].empty())
        return -1;
    if (!client.getUser().empty())
        return -2;
    const std::string ok = "[]\\`_^{}|-";
    for (size_t i = 0; i < tokens[1].size(); i++)
    {
        unsigned char c = static_cast<unsigned char>(tokens[1][i]);
        if (i == 0 && std::isdigit(c))
            return -3;
        if (!std::isalnum(c) && ok.find(c) == std::string::npos)
            return -3;
    }
    return 0;
}

int Server::parserCmdNick(const std::vector<std::string> &tokens)
{
    if (checkUserExist(tokens[1]))
		return -3;
	if (tokens.size() == 1 || tokens[1].empty())
		return -1;
	const std::string ok = "[]\\`_^{}|-";
	for (size_t i = 0; i < tokens[1].size(); i++)
	{
		char c = tokens[1][i];
		if (i == 0 && std::isdigit(c))
			return -2;
		if (!std::isalnum(c) && ok.find(c) == std::string::npos)
			return -2;
	}
	return 0;
}

int Server::parserCmdJoin(const std::vector<std::string> &tokens, Client &client)
{
	if (tokens.size() == 2 && tokens[1] == "0")
		return (2);
    if (tokens[1][0] != '#' || tokens[1].size() > 50)
        return (-5);
    if (tokens[1][0] == '#' && tokens[1].size() <= 50)
    {
        bool exist = checkChannelExist(tokens[1]);
        if (exist == false)
            return 1;
        else
        {
            bool *modes = this->_listChannel[tokens[1]]->whichMod();
            if (modes[LIMIT] && static_cast<int>(this->_listChannel[tokens[1]]->getMemberList().size()) >= this->_listChannel[tokens[1]]->getUserLimit())
                return(-1);
            if (modes[INVITE_ONLY])
                if (this->_listChannel[tokens[1]]->isInvited(client) == false)
                    return (-2);
            if (modes[PASSWORD] && tokens.size() > 2 && tokens[2].empty() == false)
            {
                if (this->_listChannel[tokens[1]]->getPassword() != tokens[2])
                    return (-3);
            }
            return (0);
        }
    }
	if (tokens.size() == 1)
		return(-4);
    return(0);
}

int Server::parserCmdJoinMulti(const std::vector<std::string> &tokens, Client &client)
{
    std::vector<std::string> passwords;
    std::vector<std::string> channels = tokenComma(tokens[1]);
    if (tokens.size() > 2)
        passwords = tokenComma(tokens[2]);
    for (size_t i = 0; i < channels.size(); i++)
    {
        const std::string& chan = channels[i];
        std::string pwd;
        if (i < passwords.size())
            pwd = passwords[i];
        else
            pwd = "";
        if (chan.empty() || chan[0] != '#')
            continue;
        bool exist = checkChannelExist(chan);
        if (exist == false)
        {
            if (chan.size() <= 50)
                cmdJoin(client, chan, true);
        }
        if (this->_listChannel[chan]->isMember(client) == true)
            continue ;  
        else
        {
            bool *modes = this->_listChannel[chan]->whichMod();
            if (modes[INVITE_ONLY])
            {
                if (this->_listChannel[chan]->isInvited(client) == false)
                {
					sendMsg(ERR_INVITEONLYCHAN("server", client.getNick(), chan), client);
                    continue ;
                }
            }
            if (modes[LIMIT] && static_cast<int>(this->_listChannel[chan]->getMemberList().size()) >= this->_listChannel[chan]->getUserLimit())
            {
				sendMsg(ERR_CHANNELISFULL("server", client.getNick(), chan), client);
                continue ;
            }
            if (modes[PASSWORD] && !pwd.empty())
            {
                if (this->_listChannel[chan]->getPassword() != pwd)
                {
					sendMsg(ERR_BADCHANNELKEY("server", client.getNick(), chan), client);
                    i++;
                    continue ;
                }
            }
            cmdJoin(client, chan, false);
        }
    }
    return (0);
}

int Server::parserCmdPart(const std::vector<std::string> &tokens, const Client& client)
{
    if (tokens.size() == 1)
        return (-1);
    else if (checkChannelExist(tokens[1]) == false || this->_listChannel[tokens[1]]->isMember(client) == false)
        return (-2);
    if (tokens.size() > 2)
        return (1);
    return (0);
}

std::vector<std::string> Server::parserCmdPartMulti(std::vector<std::string> &tokens, Client &client)
{
    std::vector<std::string> chan = tokenComma(tokens[1]);
    std::vector<std::string> chanVal;
    for (size_t i = 0; i < chan.size(); i++)
    {
        if (chan[i].empty() || chan[i][0] != '#')
            continue ;
        else if (checkChannelExist(chan[i]) == false)
			sendMsg(ERR_NOSUCHCHANNEL("server", chan[i]), client);
        else if (this->_listChannel[chan[i]]->isMember(client) == false)
            sendMsg(ERR_NOTONCHANNEL("server", client.getNick(), chan[i]), client);
        else
            chanVal.push_back(chan[i]);
    }
    if (tokens.size() > 2)
    {
        std::string reason = tokens[2];
        for (size_t i = 3; i < tokens.size(); i++)
            reason += " " + tokens[i];
        tokens.clear();
        tokens.push_back(reason);
    }
    else
        tokens.clear();
    return (chanVal);
}

int Server::parserCmdPrivMsg(const std::vector<std::string> &tokens)
{
    if (tokens.size() < 2)
        return (-1);
    if (tokens.size() < 3)
        return (-2);
    if (checkUserExist(tokens[1]))
		return (0);
    if (checkChannelExist(tokens[1]))
		return (1);
	return (-3);
}

int Server::parserCmdPrivMsgMulti(const std::vector<std::string> &tokens, Client &client)
{
    if (tokens.size() < 2)
    {
        std::string nick = client.getNick().empty() ? std::string("*") : client.getNick();
        sendMsg(ERR_NORECIPIENT("server", nick), client);
        return (-1);
    }
    if (tokens.size() < 3)
    {
        std::string nick = client.getNick().empty() ? std::string("*") : client.getNick();
        sendMsg(ERR_NOTEXTOSEND("server", nick), client);
        return (-2);
    }
    std::vector<std::string> targets = tokenComma(tokens[1]);
    for (size_t i = 0; i < targets.size(); i++)
    {
        const std::string& target = targets[i];
        if (target.empty())
            continue;
        bool userExist = checkUserExist(target);
        bool channelExist = checkChannelExist(target);
        if (!userExist && !channelExist)
			sendMsg(ERR_NOSUCHNICK("server", target), client);
        else if (userExist)
            sendMsg(MSG_PRIVMSG(client.getNick(), "realuser", client.getAddress(), target, tokens[2]), getClient(target));
        else
            sendMsgChan(MSG_PRIVMSG(client.getNick(), "realuser", client.getAddress(), target, tokens[2]), *this->_listChannel[target], client.getFd());
    }
    return (0);
}

int Server::parserCmdKick(const std::vector<std::string> &tokens, Client &client)
{
	if (tokens.size() < 3)
		return -1;
	else if (checkChannelExist(tokens[1]) == false)
        return (-2);
	else if (getChannel(tokens[1]).isMember(client) == false || checkUserExist(tokens[2]) == false)
		return -3;
	else if (getChannel(tokens[1]).isMember(getClient(tokens[2])) == false)
		return -4;
    else
    {
        if (this->_listChannel[tokens[1]]->getStatusClient(client) == OP && tokens.size() == 3)
            return (0);
        else if (this->_listChannel[tokens[1]]->getStatusClient(client) == OP && tokens.size() > 3)
            return (1);
        else
            return (-5);
    }
}

int Server::parserCmdKickMulti(const std::vector<std::string> &tokens, Client &client, bool multiUser)
{
    if (tokens.size() < 3)
    {
        std::string nick;
        if (client.getNick().empty())
            nick = "*";
        else
            nick = client.getNick();        sendMsg(ERR_NEEDMOREPARAMS("server", nick, "KICK"), client);
        return (-1);
    }
    std::vector<std::string> channels = tokenComma(tokens[2]);
    std::vector<std::string> users = tokenComma(tokens[1]);
    if (multiUser == false)
    {
        if (channels.size() != users.size())
        {
            sendMsg(ERR_NEEDMOREPARAMS("server", client.getNick(), "KICK"), client);
            return (-1);
        }
        for (size_t i = 0; i < channels.size(); i++)
        {
            const std::string& chan = channels[i];
            const std::string& user = users[i];
            if (chan.empty() || chan[0] != '#')
                continue ;
            if (checkChannelExist(chan) == false)
            {   
                sendMsg(ERR_NOSUCHCHANNEL("server", chan), client);
                continue ;
            }
            if (this->_listChannel[chan]->isMember(client) == false)
            {
                sendMsg(ERR_NOTONCHANNEL("server", client.getNick(), chan), client);
                continue ;
            }
            if (this->_listChannel[chan]->getStatusClient(client) != OP)
            {
                sendMsg(ERR_CHANOPRIVSNEEDED("server", client.getNick(), chan), client);
                continue ;
            }
            if (user.empty() || checkUserExist(user) == false)
            {
                sendMsg(ERR_NOSUCHNICK("server", user), client);
                continue ;
            }
            if (this->_listChannel[chan]->isMember(getClient(user)) == false)
            {
                sendMsg(ERR_USERNOTINCHANNEL("server", user, chan), client);
                continue ;
            }
            cmdKick(tokens, getClient(user), *this->_listChannel[chan], false);
        }
    }
    else
    {
        const std::string& user = users[0];
        if (user.empty() || checkUserExist(user) == false)
        {
            sendMsg(ERR_NOSUCHNICK("server", user), client);
            return (-2);
        }
        for (size_t i = 0; i < channels.size(); i++)
        {
            const std::string& chan = channels[i];

            if (chan.empty() || chan[0] != '#')
                continue ;
            if (checkChannelExist(chan) == false)
            {
                sendMsg(ERR_NOSUCHCHANNEL("server", chan), client);
                continue ;
            }
            if (this->_listChannel[chan]->isMember(client) == false)
            {
                sendMsg(ERR_NOTONCHANNEL("server", client.getNick(), chan), client);
                continue ;
            }
            if (this->_listChannel[chan]->getStatusClient(client) != OP)
            {
                sendMsg(ERR_CHANOPRIVSNEEDED("server", client.getNick(), chan), client);
                continue ;
            }
            if (this->_listChannel[chan]->isMember(getClient(user)) == false)
            {
                sendMsg(ERR_USERNOTINCHANNEL("server", user, chan), client);
                continue ;
            }
            cmdKick(tokens, getClient(user), *this->_listChannel[chan], false);
        }
    }
    return (0);
}

int Server::parserCmdInvite(const std::vector<std::string> &tokens, Client &client)
{
	if (tokens.size() < 3)
		return -1;
	else if (checkUserExist(tokens[1]) == false)
        return (-2);
	else if (checkChannelExist(tokens[2]) == false)
        return(-3);
	Channel *channel = _listChannel[tokens[2]];
	if (channel->isMember(client) == false)
		return -4;
    else if(channel->getModList()[INVITE_ONLY] && channel->getStatusClient(client) != OP )
        return(-5);
	else if (channel->isMember(getClient(tokens[1])))
		return (-6);
    else
    {
        this->_listChannel[tokens[2]]->setStatusClient(getClient(tokens[1]), INVITED);
        return (0);
    }
}

int Server::parserCmdTopic(const std::vector<std::string> &tokens, const Client& client)
{
	if (tokens.size() <= 1)
		return -1;
    if (checkChannelExist(tokens[1]) == false)
        return -2;
    else
    {
        Channel &Chan = getChannel(tokens[1]);
		if (tokens.size() == 2)
		{
            if (Chan.getTopic().empty())
				sendMsg(RPL_NOTOPIC("server", client.getNick(), tokens[1]), client);
			sendMsg(RPL_TOPIC("server", client.getNick(), tokens[1], Chan.getTopic()), client);
			return (1);
		}
        bool    *tab = Chan.getModList();
        if (tab[TOPIC_OPE] == true)
        {
            if (Chan.getStatusClient(client) != OP)
                return(-3);
        }
        return(0);
    }
}

int Server::parserCmdMode(const std::vector<std::string> &tokens, Client &client)
{
    if (tokens.size() < 2)
        return (-1);
    if (checkChannelExist(tokens[1]) == false)
        return(-2);
    Channel &channel = getChannel(tokens[1]);
	if (tokens.size() == 2)
	{
        sendMsg(RPL_CHANNELMODEIS("server", client.getNick(), tokens[1], "+"), client);
        sendMsg(RPL_CREATIONTIME("server", client.getNick(), tokens[1], "0"), client);
		return (1);
	}
    if (!channel.isOp(client))
        return (-3);
    std::string modes = tokens[2];
    size_t paramIndex = 3;
    char sign = 0;
    for (size_t i = 0; i < modes.size(); i++)
    {
        if (modes[i] == '+' || modes[i] == '-')
        {
            sign = modes[i];
            continue;
        }
		else if (i == 0)
			return (-5);
        char mode = modes[i];
        if (mode != 'i' && mode != 't' && mode != 'k' && mode != 'l' && mode != 'o')
            return (-4);

        std::string param;

        if (mode == 'o' || mode == 'k' || mode == 'l')
        {
            if (paramIndex >= tokens.size())
                return (-2);
            param = tokens[paramIndex++];
        }
        if (sign == '+')
            addMode(&channel, mode, param, client);
        else if (sign == '-')
            removeMode(&channel, mode, client);
        else
            return (-5);
    }
    return (0);
}

int Server::parserCmdQuit(const std::vector<std::string> &tokens)
{
    if (tokens.size() < 2)
        return(0);
    else
        return(1);
}
