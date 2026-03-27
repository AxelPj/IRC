#include "Server.hpp"

int    Server::choiceParser(const std::vector<std::string> &tokens)
{
    std::string cmdString[12] =
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
        "PONG",
        "UNKNOWN"
    };

    for(int i = 0; i < 12; i++)
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
        case 0:
            flag = parserCmdNick(tokens);
            if (flag == 0)
                cmdNick(client, tokens);
            else
                sendMsg("Usage: NICK <nickname>, sets your nick\r\n", client.getFd());
            break ;
        case 1:
            if (tokens[1].find(',') != std::string::npos)
                flag = parserCmdJoinMulti(tokens, client);
            else
            {
                flag = parserCmdJoin(tokens, client);
                if (flag == -2)
                    sendMsg(":server 473 " + client.getNick() + " :Cannot join channel (+i)\r\n", client.getFd());
                else if (flag == -3)
                    sendMsg(":server 475 " + client.getNick() + " :Cannot join channel (+k)\r\n", client.getFd());
                else if (flag == -4)
                    sendMsg(":server 471 " + client.getNick() + " " + tokens[1] + " :Cannot join channel (+l)\r\n", client.getFd());
            }
            break;
        case 2:
            if (tokens[1].find(',') != std::string::npos)
                flag = parserCmdPartMulti(tokens, client);
            else
            {
                flag = parserCmdPart(tokens);
                if (flag == 0)
                    cmdPart(client, tokens, 0);
                else if (flag == 1)
                    cmdPart(client, tokens, 1);
                else if (flag == -1)
                    sendMsg(":server 461 " + client.getNick() + " PART :Not enough parameters\r\n", client.getFd());
                else if (flag == -2)
                    sendMsg(":server 403 " + client.getNick() + " " + tokens[1] + " :No such channel\r\n", client.getFd());
            }
            break ;
        case 3:
            if (tokens[1].find(',') != std::string::npos)
                parserCmdPrivMsgMulti(tokens, client);
            else
            {
                flag = parserCmdPrivMsg(tokens);
                if (flag == 0)
                    sendMsg(tokens[2], getClient(tokens[1]).getFd());
                else if (flag == 1)
                    sendMsgChan(tokens[2], *this->_listChannel[tokens[1]], client.getFd());
                else if (flag == -1)
                    sendMsg("Usage: PRIVMSG <target> <message>\r\n", client.getFd());
                else if (flag == -2)
                    sendMsg("Error: the user does not exist\r\n", client.getFd());
            }
            break ;
        case 4:
            flag = parserCmdKick(tokens, client);
            if (flag == 0)
                cmdKick(tokens, getClient(tokens[1]), *this->_listChannel[tokens[1]], false); 
            else if (flag == 1)
                cmdKick(tokens, getClient(tokens[1]), *this->_listChannel[tokens[1]], true);
            else if (flag == -1) 
                sendMsg(":server 401 " + client.getNick() + " " + tokens[1] + " :No such nick/channel\r\n", client.getFd());
            else if (flag == -2)
                sendMsg(":server 482 " + client.getNick() + " " + tokens[1] + " :You are not channel operator\r\n", client.getFd());
            break ;
        case 5:
            flag = parserCmdInvite(tokens, client);
            if (flag == 0)
                cmdInvite(client, tokens);
            else if (flag == -1)
                sendMsg(":server 401 " + client.getNick() + " " + tokens[1] + " :No such nick/channel\r\n", client.getFd());
            else if (flag == -2)
                sendMsg(":server 482 " + client.getNick() + " " + tokens[2] + " :You are not channel operator\r\n", client.getFd());
            else
                sendMsg("Usage: INVITE <user> <channel>\r\n", client.getFd());
            break ;
        case 6:
            flag = parserCmdTopic(tokens, client);
            if (flag == 0)
                cmdTopic(tokens);
            else if (flag == -1)
                sendMsg("does not exist\r\n", client.getFd());
            else if (flag == -2)
                sendMsg(":server 482 " + client.getNick() + " " + tokens[1] + " :You are not channel operator\r\n", client.getFd());
            break ;
        case 7:
            flag = parserCmdMode(tokens, client);
            if (flag == 0)
                cmdMode(client, tokens);
            else
                sendMsg("Usage: MODE <channel> <modes>\r\n", client.getFd());
            break ;
        case 8:
            flag = parserCmdQuit(tokens, client);
            if (flag == 0)
                cmdQuit(client, "");
            if (flag == 1)
                cmdQuit(client, tokens[1]);
            break ;
        case 9:
            cmdPong(client, tokens);
            break ;
        case 10:
            cmdPing(client, tokens);
            break ;
        default:
            sendMsg("Unknown command\r\n", client.getFd());
            break;
    }
}

int Server::parserCmdNick(const std::vector<std::string> &tokens) const
{
    if (tokens.size() == 2 && !tokens[1].empty())
    {
        const std::string ok = "[]\\`_^{}|-";
        for (size_t i = 0; i < tokens[1].size(); ++i)
        {
            char c = tokens[1][i];
            if (i == 0 && std::isdigit(c))
                return -1;
            if (!std::isalnum(c) && ok.find(c) == std::string::npos)
                return -1;
        }
        return 0;
    }
    return -1;
}

int Server::parserCmdJoin(const std::vector<std::string> &tokens, Client &client)
{
    if (tokens[1][0] == '#' && tokens[1].size() <= 50)
    {
        bool exist = checkChannelExist(tokens[1]);
        if (exist == false)
            return(1);
        else if (exist == true)
        {
            bool *modes = this->_listChannel[tokens[1]]->whichMod();
            if (modes[LIMIT] && this->_listChannel[tokens[1]]->getMemberList().size() >= this->_listChannel[tokens[1]]->getUserLimit())
                return(-4);
            if (modes[INVITE_ONLY])
                if (this->_listChannel[tokens[1]]->isInvited(client) == false)
                    return (-2);
            if (modes[PASSWORD] && tokens.size() > 2 && tokens[2].empty() == false)
            {
                if (this->_listChannel[tokens[1]]->getPassword( ) != tokens[2])
                    return -3;
            }
            return (0);
        }
    }
    return(-1);
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
                    sendMsg(":server 473 " + client.getNick() + " " + chan + " :Cannot join channel (+i)\r\n", client.getFd());
                    continue ;
                }
            }
            if (modes[LIMIT] && this->_listChannel[chan]->getMemberList().size() >= this->_listChannel[chan]->getUserLimit())
                sendMsg(":server 471 " + client.getNick() + " " + chan + " :Cannot join channel (+l)\r\n", client.getFd());
            if (modes[PASSWORD] && !pwd.empty())
            {
                if (this->_listChannel[chan]->getPassword() != pwd)
                {
                    sendMsg(":server 475 " + client.getNick() + " " + chan + " :Cannot join channel (+k)\r\n", client.getFd());
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
    if (tokens[1][0] != '#')
        return (-1);
    else if (checkChannelExist(tokens[1]) == false)
        return (-2);
    if (tokens.size() > 2)
        return (1);
    return (0);
}

int Server::parserCmdPrivMsg(const std::vector<std::string> &tokens)
{
    if (tokens.size() < 3)
        return (-1);
    bool userExist = checkUserExist(tokens[1]);
    bool channelExist = checkChannelExist(tokens[1]);
    if (!userExist && !channelExist)
        return (-2);
    else if (userExist)
        return(0);
    else
        return(1);
    return (0);
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
            sendMsg("Error: " + target + " does not exist\r\n", client.getFd());
        else if (userExist)
            sendMsg(tokens[2], getClient(target).getFd());
        else
            sendMsgChan(tokens[2], *this->_listChannel[target], client.getFd());
    }
    return (0);
}

int Server::parserCmdKick(const std::vector<std::string> &tokens, Client &client)
{
    if (checkChannelExist(tokens[1]) == false || getChannel(tokens[1]).isMember(getClient(tokens[2])) == false)
        return (-1);
    else
    {
        if (this->_listChannel[tokens[1]]->getStatusClient(client) == OP && tokens.size() == 3)
            return (0);
        else if (this->_listChannel[tokens[1]]->getStatusClient(client) == OP && tokens.size() > 3)
            return (1);
        else
            return (-2);
    }
}

int Server::parserCmdInvite(const std::vector<std::string> &tokens, Client &client)
{
    if (checkUserExist(tokens[1]) == false)
        return (-1);
    bool channelExist = checkChannelExist(tokens[2]);
    if (channelExist == false)
        return(-1);
    else if(this->_listChannel[tokens[2]]->getStatusClient(client) != OP )
        return(-2);
    else
    {
        this->_listChannel[tokens[2]]->setStatusClient(getClient(tokens[1]), INVITED);
        return (0);
    }
}

int Server::parserCmdTopic(const std::vector<std::string> &tokens, const Client& client)
{
    if (checkChannelExist(tokens[1]) == false)
        return (-1);
    else
    {
        Channel &Chan = getChannel(tokens[1]);
        bool    *tab = Chan.getModList();
        if (tab[TOPIC_OPE] == true)
        {
            if (Chan.getStatusClient(client) != OP)
                return(-2);
        }
        return(0);
    }
}

int Server::parserCmdMode(const std::vector<std::string> &tokens, Client &client)
{
    if (tokens.size() < 3)
        return (-1);
    if (checkChannelExist(tokens[1]) == false)
        return(-1);
    Channel &channel = getChannel(tokens[1]);
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
        char mode = modes[i];
        if (mode != 'i' && mode != 't' && mode != 'k' && mode != 'l' && mode != 'o')
            return (-2);

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
            return (-2);
    }
    return (0);
}

int Server::parserCmdQuit(const std::vector<std::string> &tokens, const Client &client)
{
    if (tokens.size() < 2)
        return(0);
    else
        return(1);
}
