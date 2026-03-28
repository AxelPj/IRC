#include "Server.hpp"

int    Server::choiceParser(const std::vector<std::string> &tokens)
{
    std::string cmdString[11] =
    {
        "NICK",
        "JOIN",
        "PART",
        "PRIVMSG",
        "KICK",
        "INVITE",
        "TOPIC",
        "MODE",
        "QUIT",
        "PING",
        "PONG"
    };

    for(int i = 0; i < 11; i++)
    {
        if (tokens[0] == cmdString[i])
            return (i);
    }
    return(-1);
}

void    Server::processParser(Client &client)
{
    int flag;
    std::vector<std::string> tokens = tokenSpace(client.getBuffer());
    if (tokens.empty() == true)
        return ;
    for (size_t i = 0; i < tokens[0].size(); i++)
        tokens[0][i] = toupper(tokens[0][i]); 
    if (tokens.size() == 1 && tokens[0] != "TOPIC" && tokens[0] != "QUIT")
                return ;
    switch(choiceParser(tokens))
    {
        case 0: //NICK
            flag = parserCmdNick(tokens);
			if (flag == -1)
				sendMsg(ERR_NONICKNAMEGIVEN(), client.getFd());
			if (flag == -2)
				sendMsg(ERR_ERRONEUSNICKNAME("server", client.getNick()), client.getFd());
			if (flag == -3)
				sendMsg(ERR_NICKNAMEINUSE(), client.getFd());
            if (flag == 0)
                cmdNick(client, tokens);
            else
                sendMsg("Usage: NICK <nickname>, sets your nick\r\n", client.getFd());
            break ;
        case 1: //JOIN
            if (tokens[1].find(',') != std::string::npos)
                flag = parserCmdJoinMulti(tokens, client);
            else
            {
                flag = parserCmdJoin(tokens, client);
                if (flag == -2)
                    sendMsg(ERR_INVITEONLYCHAN("server", client.getNick(), tokens[1]), client.getFd());
                else if (flag == -3)
                    sendMsg(ERR_BADCHANNELKEY("server", client.getNick(), tokens[1]), client.getFd());
				else if (flag == -4)
					sendMsg(ERR_NEEDMOREPARAMS(), client.getFd());
                else if (flag == -1)
                    sendMsg(ERR_CHANNELISFULL("server", client.getNick(), tokens[1]), client.getFd());
				else if (flag == 1)
					//create channel
				else if (flag == 2)
					//leave all channels
				if (flag == 0 || flag == 1)
					//join channel
					//cmdJoin(client, tokens[1], true)
				else //Wrong channel name
					sendMsg(ERR_NOSUCHCHANNEL(), client.getFd());
            }
            break;
        case 2: //PART
            if (tokens[1].find(',') != std::string::npos)
                flag = parserCmdPartMulti(tokens, client);
            else
            {
                flag = parserCmdPart(tokens);
                if (flag == 0)
                    cmdPart(client, tokens, 0); // no reason
                else if (flag == 1)
                    cmdPart(client, tokens, 1); // reason
                else if (flag == -1)
                    sendMsg(ERR_NEEDMOREPARAMS("server", client.getNick(), "PART"), client.getFd());
                else if (flag == -2)
					sendMsg(ERR_NOSUCHCHANNEL("server", client.getNick(), tokens[1]), client.getFd());
				else if (flag == -3)
					sendMsg(ERR_NOTONCHANNEL("server", client.getNick(), tokens[1]), client.getFd());
            }
            break ;
        case 3: //PRIVMSG
            if (tokens[1].find(',') != std::string::npos)
                parserCmdPrivMsgMulti(tokens, client);
            else
            {
                flag = parserCmdPrivMsg(tokens);
                if (flag == 0)
                    sendMsg(tokens[2], getClient(tokens[1]).getFd());
                else if (flag == 1) //TODO Add 404 CANNOTSENDTOCHAN somehow
                    sendMsgChan(tokens[2], *this->_listChannel[tokens[1]], client.getFd());
                else if (flag == -1)
                    sendMsg(ERR_NORECIPIENT("server", client.getNick()), client.getFd());
                else if (flag == -2)
                    sendMsg(ERR_NOTEXTOSEND("server", client.getNick()), client.getFd());
                else if (flag == -3)
                    sendMsg(ERR_NOSUCHNICK("server", tokens[1]), client.getFd());
            }
            break ;
        case 4: //KICK
				//TODO parserCmdKickMulti
            flag = parserCmdKick(tokens, client);
			if (flag == -1)
				sendMsg(ERR_NEEDMOREPARAMS(), client.getFd());
            else if (flag == -2) 
				sendMsg(ERR_NOSUCHCHANNEL("server", tokens[1]), client.getFd());
            else if (flag == -3) 
				sendMsg(ERR_NOTONCHANNEL(), client.getFd());
			else if (flag == -4)
				sendMsg(ERR_USERNOTINCHANNEL(), client.getFd());
            else if (flag == -5)
                sendMsg(ERR_CHANOPRIVSNEEDED("server", client.getNick(), tokens[1]), client.getFd());
            if (flag == 0)
                cmdKick(tokens, getClient(tokens[1]), *this->_listChannel[tokens[1]], false); 
            else // (flag == 1)
                cmdKick(tokens, getClient(tokens[1]), *this->_listChannel[tokens[1]], true);
            break ;
        case 5: //INVITE
            flag = parserCmdInvite(tokens, client);
            if (flag == -1)
                sendMsg(ERR_NOSUCHNICK("server", client.getNick(), tokens[1]), client.getFd());
			else if (flag == -2)
				sendMsg(ERR_NOSUCHCHANNEL("server", client.getNick(), tokens[2]), client.getFd());
            else if (flag == -3)
				sendMsg(ERR_NOTONCHANNEL("server", client.getNick(), tokens[2]), client.getFd());
            else if (flag == -4)
                sendMsg(ERR_CHANOPRIVSNEEDED("server", client.getNick(), tokens[1]), client.getFd());
			else if (flag == -5)
				sendMsg(ERR_USERONCHANNEL("server", client.getNick(), tokens[2], tokens[1]), client.getFd());
			else
				cmdInvite(client, tokens);
            break ;
        case 6: //TOPIC
            flag = parserCmdTopic(tokens, client);
            if (flag == 0)
                cmdTopic(tokens);
			else if (flag == -1)
				sendMsg(ERR_NEEDMOREPARAMS("server", client.getNick(), tokens[1]), client.getFd());
            else if (flag == -2)
				sendMsg(ERR_NOSUCHCHANNEL("server", tokens[1]), client.getFd());
            else if (flag == -3)
                sendMsg(ERR_CHANOPRIVSNEEDED("server", client.getNick(), tokens[1]), client.getFd());
            break ;
        case 7: //MODE
            flag = parserCmdMode(tokens, client);
			if (flag == -1)
				sendMsg(ERR_NEEDMOREPARAMS("server", client.getNick(), tokens[0]), client.getFd());
			else if (flag == -2)
                sendMsg(ERR_NOSUCHCHANNEL("server", tokens[1]), client.getFd());
			else if (flag == -3)
                sendMsg(ERR_CHANOPRIVSNEEDED("server", client.getNick(), tokens[1]), client.getFd());
			else if (flag == -4)
				sendMsg(ERR_UNKNOWNMODE("server", client.getNick()), client.getFd(), tokens[2]);
			else if (flag == -5)
				sendMsg(ERR_INVALIDMODEPARAM(), client.getFd());
            break;
        case 8: //QUIT
            flag = parserCmdQuit(tokens);
            if (flag == 0)
                cmdQuit(client, "");
            if (flag == 1)
                cmdQuit(client, tokens[1]);
            break ;
        case 9: //PING
            cmdPong(client, tokens);
            break ;
        case 10: //PONG
            cmdPing(client, tokens);
            break ;
        default: //UNKNOWN
            sendMsg(ERR_UNKNOWNCOMMAND("server", tokens[0]), client.getFd());
            break;
    }
}

int Server::parserCmdNick(const std::vector<std::string> &tokens) const
{
	if (tokens.size() <= 1 || tokens[1].empty())
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
	if checkUserExist(tokens[1])
		return -3;
	return 0;
}

int Server::parserCmdJoin(const std::vector<std::string> &tokens, Client &client)
{
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
                    return -3;
            }
            return (0);
        }
    }
	if (tokens.size() == 2 && tokens[1] == "0")
		return (2);
	if (tokens.size() == 1)
		return(-4);
    return(-5);
}

int Server::parserCmdJoinMulti(const std::vector<std::string> &tokens, Client &client)
{
    if (tokens.size() < 2)
        return -1;
    std::vector<std::string> passwords;
    std::vector<std::string> channels = tokenComma(tokens[1]);
    if (tokens.size() > 2)
        passwords = tokenComma(tokens[2]);
    size_t i = 0;
    while (i < channels.size())
    {
        const std::string& chan = channels[i];
        std::string pwd;
        if (i < passwords.size())
            pwd = passwords[i];
        else
            pwd = "";
        if (chan.empty() || chan[0] != '#')
            return -1;
        bool exist = checkChannelExist(chan);
        if (exist == false)
        {
            if (chan.size() <= 50)
                cmdJoin(client, chan, true);
        }
        else
        {
            bool *modes = this->_listChannel[chan]->whichMod();
            if (modes[INVITE_ONLY])
            {
                if (this->_listChannel[chan]->isInvited(client) == false)
                {
					sendMsg(ERR_INVITEONLYCHAN("server", client.getNick(), chan), client.getFd());
                    continue ;
                }
            }
            if (modes[LIMIT] && static_cast<int>(this->_listChannel[chan]->getMemberList().size()) >= this->_listChannel[chan]->getUserLimit())
				sendMsg(ERR_CHANNELISFULL("server", client.getNick(), chan), client.getFd());
            if (modes[PASSWORD] && !pwd.empty())
            {
                if (this->_listChannel[chan]->getPassword() != pwd)
                {
					sendMsg(ERR_BADCHANNELKEY("server", client.getNick(), chan), client.getFd());
                    i++;
                    continue ;
                }
            }
            cmdJoin(client, chan, true);
        }
        i++;
    }
    return (0);
}

int Server::parserCmdPart(const std::vector<std::string> &tokens)
{
    if (tokens.size() == 1)
        return (-1);
    else if (checkChannelExist(tokens[1]) == false)
        return (-2);
	//else if user not on channel
	//return -3;
    if (tokens.size() > 2)
        return (1);
    return (0);
}

int Server::parserCmdPartMulti(const std::vector<std::string> &tokens, Client &client)
{
    std::vector<std::string> channels = tokenComma(tokens[1]);
    for (size_t i = 0; i < channels.size(); i++)
    {
        std::vector<std::string> chan;
		//TODO pushback reason if existing here
        chan.push_back(channels[i]);
        if (chan[0].empty() || chan[0][0] != '#')
            continue ;
        if (checkChannelExist(chan[0]) == false)
			sendMsg(ERR_NOSUCHCHANNEL("server", chan[0]), client.getFd());
        else
            cmdPart(client, chan, 1);
    }
    return (0);
}

int Server::parserCmdPrivMsg(const std::vector<std::string> &tokens)
{
    if (tokens.size() < 2)
        return (-1);
    if (tokens.size() < 3)
        return (-2);
	if checkUserExist(tokens[1])
		return (0);
	if checkChannelExist(tokens[1])
		return (1);
	return (-3);
}

int Server::parserCmdPrivMsgMulti(const std::vector<std::string> &tokens, Client &client)
{
    std::vector<std::string> targets = tokenComma(tokens[1]);   
    for (size_t i = 0; i < targets.size(); i++)
    {
        const std::string& target = targets[i];
        if (target.empty())
            continue;
        bool userExist = checkUserExist(target);
        bool channelExist = checkChannelExist(target);
        if (!userExist && !channelExist)
			sendMsg(ERROR_NOSUCHNICK("server", client.getNick(), target), client.getFd());
        else if (userExist)
            sendMsg(tokens[2], getClient(target).getFd());
        else //TODO Add 404 CANNOTSENDTOCHAN somehow
            sendMsgChan(tokens[2], *this->_listChannel[target], client.getFd());
    }
    return (0);
}

int Server::parserCmdKick(const std::vector<std::string> &tokens, Client &client)
{
	if (tokens.size() < 3)
		return -1;
	else if (checkChannelExist(tokens[1]) == false)
        return (-2);
	else if (getChannel(tokens[1]).isMember(client) == false)
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

int Server::parserCmdInvite(const std::vector<std::string> &tokens, Client &client)
{
    if (checkUserExist(tokens[1]) == false)
        return (-1);
	else if (checkChannelExist(tokens[2]) == false)
        return(-2);
	//else if client not on Channel
	//return -3
    else if(this->_listChannel[tokens[2]]->getStatusClient(client) != OP )
        return(-4);
	//else if tokens[1] on Channel
	//return -5
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
			if (server.getTopic().empty())
				sendMsg(RPL_NOTOPIC("server", client.getNick(), tokens[1]), client.getFd());
			sendMsg(RPL_TOPIC("server", client.getNick(), tokens[1], Chan.getTopic()), client.getFd());
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
		sendMsg(RPL_CHANNELMODEIS(), client.getFd());
		sendMsg(RPL_CREATIONTIME(), client.getFd());
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
