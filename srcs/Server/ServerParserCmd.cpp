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
    if (userExists(tokens[1]))
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
        bool exist = channelExists(tokens[1]);
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
            if (modes[PASSWORD])
            {
                if (tokens.size() <= 2 || tokens[2].empty() || this->_listChannel[tokens[1]]->getPassword() != tokens[2])
                    return (-3);
            }
            return (0);
        }
    }
	if (tokens.size() == 1)
		return (-4);
    return (0);
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
        bool exist = channelExists(chan);
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
					sendMsg(ERR_INVITEONLYCHAN(SERVER_NAME, client.getNick(), chan), client);
                    continue ;
                }
            }
            if (modes[LIMIT] && static_cast<int>(this->_listChannel[chan]->getMemberList().size()) >= this->_listChannel[chan]->getUserLimit())
            {
				sendMsg(ERR_CHANNELISFULL(SERVER_NAME, client.getNick(), chan), client);
                continue ;
            }
            if (modes[PASSWORD])
            {
                if (pwd.empty() || this->_listChannel[chan]->getPassword() != pwd)
                {
					sendMsg(ERR_BADCHANNELKEY(SERVER_NAME, client.getNick(), chan), client);
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
    else if (channelExists(tokens[1]) == false || this->_listChannel[tokens[1]]->isMember(client) == false)
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
        else if (channelExists(chan[i]) == false)
			sendMsg(ERR_NOSUCHCHANNEL(SERVER_NAME, chan[i]), client);
        else if (this->_listChannel[chan[i]]->isMember(client) == false)
            sendMsg(ERR_NOTONCHANNEL(SERVER_NAME, client.getNick(), chan[i]), client);
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
    if (userExists(tokens[1]))
		return (0);
    if (channelExists(tokens[1]))
		return (1);
	return (-3);
}

int Server::parserCmdPrivMsgMulti(const std::vector<std::string> &tokens, Client &client)
{
    if (tokens.size() < 2)
    {
        std::string nick = client.getNick().empty() ? std::string("*") : client.getNick();
        sendMsg(ERR_NORECIPIENT(SERVER_NAME, nick), client);
        return (-1);
    }
    if (tokens.size() < 3)
    {
        std::string nick = client.getNick().empty() ? std::string("*") : client.getNick();
        sendMsg(ERR_NOTEXTOSEND(SERVER_NAME, nick), client);
        return (-2);
    }
    std::vector<std::string> targets = tokenComma(tokens[1]);
    for (size_t i = 0; i < targets.size(); i++)
    {
        const std::string& target = targets[i];
        if (target.empty())
            continue;
        if (userExists(target))
            sendMsg(MSG_PRIVMSG(client.getNick(), client.getUser(), client.getAddress(), target, tokens[2]), getClient(target));
        else if (channelExists(target))
            sendMsgChan(MSG_PRIVMSG(client.getNick(), client.getUser(), client.getAddress(), target, tokens[2]), *this->_listChannel[target], client.getFd());
		else
			sendMsg(ERR_NOSUCHNICK(SERVER_NAME, target), client);
    }
    return (0);
}

int Server::parserCmdKick(const std::vector<std::string> &tokens, Client &client)
{
	if (channelExists(tokens[0]) == false)
        return (-2);
	else if (getChannel(tokens[0]).isMember(client) == false || userExists(tokens[1]) == false)
		return -3;
	else if (getChannel(tokens[0]).isMember(getClient(tokens[1])) == false)
		return -4;
    else
    {
        if (this->_listChannel[tokens[0]]->getStatusClient(client) == OP && tokens.size() == 2)
            return (0);
        else if (this->_listChannel[tokens[0]]->getStatusClient(client) == OP && tokens.size() > 2)
            return (1);
        else
            return (-5);
    }
}

int Server::parserCmdKickMulti(const std::vector<std::string> &tokens, Client &client, bool multiUser)
{
    std::vector<std::string> channels = tokenComma(tokens[0]);
    std::vector<std::string> users = tokenComma(tokens[1]);
    if (multiUser == false)
    {
        if (users.size() != 1)
        {
            sendMsg(ERR_NEEDMOREPARAMS(SERVER_NAME, client.getNick(), "KICK"), client);
            return (-1);
        }
        const std::string& user = users[0];
        if (user.empty() || userExists(user) == false)
        {
            sendMsg(ERR_NOSUCHNICK(SERVER_NAME, user), client);
            return (-2);
        }
        for (size_t i = 0; i < channels.size(); i++)
        {
            const std::string& chan = channels[i];
            if (chan.empty() || chan[0] != '#')
                continue;
            if (channelExists(chan) == false)
            {
                sendMsg(ERR_NOSUCHCHANNEL(SERVER_NAME, chan), client);
                continue;
            }
            if (this->_listChannel[chan]->isMember(client) == false)
            {
                sendMsg(ERR_NOTONCHANNEL(SERVER_NAME, client.getNick(), chan), client);
                continue;
            }
            if (this->_listChannel[chan]->getStatusClient(client) != OP)
            {
                sendMsg(ERR_CHANOPRIVSNEEDED(SERVER_NAME, client.getNick(), chan), client);
                continue;
            }
            if (this->_listChannel[chan]->isMember(getClient(user)) == false)
            {
                sendMsg(ERR_USERNOTINCHANNEL(SERVER_NAME, user, chan), client);
                continue;
            }
            cmdKick(client, tokens, getClient(user), *this->_listChannel[chan], false);
        }
    }
    else
    {
        if (channels.size() != users.size())
        {
            sendMsg(ERR_NEEDMOREPARAMS(SERVER_NAME, client.getNick(), "KICK"), client);
            return (-1);
        }
        for (size_t i = 0; i < channels.size(); i++)
        {
            const std::string& chan = channels[i];
            const std::string& user = users[i];
            
            if (chan.empty() || chan[0] != '#')
                continue;
            if (userExists(user) == false)
            {
                sendMsg(ERR_NOSUCHNICK(SERVER_NAME, user), client);
                continue;
            }
            if (channelExists(chan) == false)
            {
                sendMsg(ERR_NOSUCHCHANNEL(SERVER_NAME, chan), client);
                continue;
            }
            if (this->_listChannel[chan]->isMember(client) == false)
            {
                sendMsg(ERR_NOTONCHANNEL(SERVER_NAME, client.getNick(), chan), client);
                continue;
            }
            if (this->_listChannel[chan]->getStatusClient(client) != OP)
            {
                sendMsg(ERR_CHANOPRIVSNEEDED(SERVER_NAME, client.getNick(), chan), client);
                continue;
            }
            if (this->_listChannel[chan]->isMember(getClient(user)) == false)
            {
                sendMsg(ERR_USERNOTINCHANNEL(SERVER_NAME, user, chan), client);
                continue;
            }
            cmdKick(client, tokens, getClient(user), *this->_listChannel[chan], false);
        }
    }
    return (0);
}

int Server::parserCmdInvite(const std::vector<std::string> &tokens, Client &client)
{
	if (tokens.size() < 3)
		return -1;
	else if (userExists(tokens[1]) == false)
        return (-2);
	else if (channelExists(tokens[2]) == false)
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
	if (tokens.size() == 1)
		return -1;
    if (channelExists(tokens[1]) == false)
        return -2;
    else
    {
        Channel &Chan = getChannel(tokens[1]);
		if (tokens.size() == 2)
		{
            if (Chan.getTopic().empty())
				sendMsg(RPL_NOTOPIC(SERVER_NAME, client.getNick(), tokens[1]), client);
			else
				sendMsg(RPL_TOPIC(SERVER_NAME, client.getNick(), tokens[1], Chan.getTopic()), client);
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
    if (channelExists(tokens[1]) == false)
        return(-2);
    Channel &channel = getChannel(tokens[1]);
	if (tokens.size() == 2)
	{
        sendMsg(RPL_CHANNELMODEIS(SERVER_NAME, client.getNick(), tokens[1], channel.getModeString()), client);
        sendMsg(RPL_CREATIONTIME(SERVER_NAME, client.getNick(), tokens[1], channel.getCreationTimeAsString()), client);
		return (1);
	}
    if (!channel.isOp(client))
        return (-3);
    std::string modes = tokens[2];
    char sign = 0;
    size_t paramIndex = 3;
    for (size_t i = 0; i < modes.size(); i++)
    {
        if (modes[i] == '+' || modes[i] == '-')
        {
            sign = modes[i];
            continue;
        }
		else if (i == 0)
			return (-5);
        if (sign != '+' && sign != '-')
            return (-5);
        char mode = modes[i];
        if (mode != 'i' && mode != 't' && mode != 'k' && mode != 'l' && mode != 'o')
            return (-4);
		if (sign == '+' && mode == 'k' && channel.getModList()[PASSWORD])
			return (-6);
        std::string param;
        bool needsParam = (mode == 'o') || (sign == '+' && (mode == 'k' || mode == 'l'));
        if (needsParam)
        {
            if (paramIndex >= tokens.size() || tokens[paramIndex].empty())
                return (-5);
            param = tokens[paramIndex++];
            if (mode == 'o')
            {
                if (!userExists(param) || !channel.isMember(getClient(param)))
                    return (-5);
            }
            else if (mode == 'l')
            {
                for (size_t j = 0; j < param.size(); j++)
                {
                    if (!std::isdigit(static_cast<unsigned char>(param[j])))
                        return (-5);
                }
            }
        }
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

int Server::parserCmdWho(const std::vector<std::string> &tokens)
{
	if (tokens.size() < 2)
		return -1;
	if (userExists(tokens[1]))
		return (0);
	if (channelExists(tokens[1]))
		return (1);
	return -2;
}
