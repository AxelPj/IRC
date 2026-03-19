#include "Server.hpp"

std::vector<std::string>  Server::tokenParser(std::string buffer)
{
    char *bufCpy = strdup(buffer.c_str());
    std::vector<std::string> tokens;

    char *token;
    token = strtok(bufCpy, " ");
    if (token == nullptr)
    {
        free(bufCpy);
        return (tokens);
    }
    tokens.push_back(token);
    while ((token = strtok(nullptr, " ")) != nullptr)
    {
        if (token[0] == ':')
        {
            std::string tmp;
            tmp = token + 1;
            token = strtok(nullptr, "\r\n");
            if (token != nullptr)
            {
                tmp += token;
                tokens.push_back(tmp);

            }
            free(bufCpy);
            return(tokens);
        }
        else
            tokens.push_back(token);
    }
    free(bufCpy);
    return (tokens);
}

int    Server::choiceParser(std::vector<std::string> tokens)
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
    int ret;
    int flag;
    std::vector<std::string> tokens = tokenParser(client.getBuffer());
    if (tokens.empty() == true)
        return ;
    if (tokens[0][0] == '/')
    {
        for (size_t i = 0; i < tokens[0].size(); i++)
            tokens[0][i] = toupper(tokens[0][i]);
        ret = choiceParser(tokens);
        switch(ret)
        {
            case 0:
                flag = parserCmdNick(tokens);
                if (flag == 0)
                    cmdNick(client, tokens);
                else
                    sendMsg("Usage: NICK <nickname>, sets your nick", client.getfd());
                break ;
            case 1:
                if (tokens[1].find(',#'))
                    flag = parserCmdJoinMulti(tokens);
                else
                    flag = parserCmdJoin(tokens);
                if (flag == 0)
                    //cmdJoin(client, tokens);
                else 
                    sendMsg("Usage: JOIN <channel>", client.getfd());
                break ;
            case 2:
                if (tokens[1].find(',#'))
                    flag = parserCmdPartMulti(tokens);
                else
                    flag = parserCmdPart(tokens);
                if (flag == 0)
                    cmdPart(client.getfd(), tokens, 0);
                else if (flag == 1)
                    cmdPart(client.getfd(), tokens, 1);
                else if (flag == -1)
                    sendMsg("Usage: PART [<channel>] [<reason>], leaves the channel, by default the current on", client.getfd());
                else if (flag == -2)
                    sendMsg(tokens[1] + ": No such channel", client.getfd());
                break ;
            case 3:
                flag = parserCmdPrivMsg(tokens);
                if (flag == 0)
                    //cmdPrivMsg(client, tokens);
                else
                    sendMsg("Usage: PRIVMSG <target> <message>", client.getfd());
                break ;
            case 4:
                flag = parserCmdKick(tokens);
                if (flag == 0)
                    //cmdKick(client, tokens);
                else
                    sendMsg("Usage: KICK <channel> <user> [<reason>]", client.getfd());
                break ;
            case 5:
                flag = parserCmdInvite(tokens);
                if (flag == 0)
                    //cmdInvite(client, tokens);
                else
                    sendMsg("Usage: INVITE <user> <channel>", client.getfd());
                break ;
            case 6:
                flag = parserCmdTopic(tokens);
                if (flag == 0)
                    //cmdTopic(client, tokens);
                else
                    sendMsg("Usage: TOPIC <channel> [<topic>]", client.getfd());
                break ;
            case 7:
                flag = parserCmdMode(tokens);
                if (flag == 0)
                    //cmdMode(client, tokens);
                else
                    sendMsg("Usage: MODE <channel> <modes>", client.getfd());
                break ;
            case 8:
                flag = parserCmdQuit(tokens);
                if (flag == 0)
                    //cmdQuit(client, tokens);
                break ;
            case 9:
                flag = parserCmdPing(tokens);
                if (flag == 0)
                    //cmdPing(client, tokens);
                break ;
            case 10:
                flag = parserCmdPong(tokens);
                if (flag == 0)
                    //cmdPong(client, tokens);
                break ;
            default:
                sendMsg("Unknown command", client.getfd());
                break;
        }
        //else
            //cmdPrivMsg(token);
    }
}

int parserCmdNick(std::vector<std::string> tokens)
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

int Server::parserCmdJoin(std::vector<std::string> tokens, Client &client)
{
    const std::string ok = "\\[{`_^}-]|";
    if (tokens[1][0] == '#')
    {
        for (size_t i = 0; i < tokens[1].size(); ++i)
        {
            char c = tokens[1][i];
            if (!std::isalnum(c) && ok.find(c) == std::string::npos)
                return -1;
        }
        int exist = checkChannelExist(tokens[1]);
        if (exist == -1)
            return(1);
        else if (exist == 0)
        {
            bool *modes = this->_listChannel[tokens[1]]->whichMod();
            if (modes[INVITE_ONLY])
                if (this->_listChannel[tokens[1]]->isInvited(client) == false)
                    return (-2);
            if (modes[PASSWORD] && tokens[2].empty() == false)
            {
                if (this->_listChannel[tokens[1]]->getpassword() != tokens[2])
                    return -3;
            }
            return (0);
        }
    }
    return(-1);
}

int Server::parserCmdJoinMulti(std::vector<std::string> tokens, Client &client)
{
    const std::string ok = "\\[{`_^}-]|";
    if (tokens[1][0] == '#')
    {
        for (size_t i = 0; i < tokens[1].size(); ++i)
        {
            char c = tokens[1][i];
            if (!std::isalnum(c) && ok.find(c) == std::string::npos)
                return -1;
        }
        int exist = checkChannelExist(tokens[1]);
        if (exist == -1)
        {
            return(1);
        }
        else if (exist == 0)
        {
            bool *modes = this->_listChannel[tokens[1]]->whichMod();
            if (modes[INVITE_ONLY])
                if (this->_listChannel[tokens[1]]->isInvited(client) == false)
                    return (-2);
            if (modes[PASSWORD] && tokens[2].empty() == false)
            {
                if (this->_listChannel[tokens[1]]->getpassword() != tokens[2])
                    return -3;
            }
            return (0);
        }
    }
    return (-1);
}

int Server::parserCmdPart(std::vector<std::string> tokens)
{
    if (tokens.size() > 1)
    {
        if (tokens[1][0] != '#')
            return (-1);
        if (checkChannelExist(tokens[1]) != 0)
            return (-2);
        if (tokens.size() > 2)
            return (1);
        return (0);
    }
}


int Server::parserCmdPrivMsg(std::vector<std::string> tokens)
{
    //creation d un channel private si target == client sinon chan normal si target == chan
    if (checkChannel(tokens[1]))
        return(1);
    else if ()
}

int Server::parserCmdKick(std::vector<std::string> tokens, Client &client)
{
    //verif que le client est op + chan existe + client a kick dans le chan
}

int Server::parserCmdInvite(std::vector<std::string> tokens)
{
    //verif que le client est op + chan existe + client a inviter dans le chan
}

int Server::parserCmdTopic(std::vector<std::string> tokens)
{
 //verif mode + droits op + client op si droit op
}

int Server::parserCmdMode(std::vector<std::string> tokens)
{
    //verif client est op + mode valide
}

int Server::parserCmdPing(std::vector<std::string> tokens)
{

}

int Server::parserCmdPong(std::vector<std::string> tokens)
{

}

int Server::parserCmdQuit(std::vector<std::string> tokens)
{

}

int Server::parserCmdReconnect(std::vector<std::string> tokens)
{

}

int Server::checkChannelExist(std::string channelName)
{
    if (this->_listChannel.find(channelName) != this->_listChannel.end())
        return (0);
    else
        return (-1);
}

std::vector<std::string>  Server::tokenSp(std::string buffer)
{
    char *bufCpy = strdup(buffer.c_str());
    std::vector<std::string> tokens;

    char *token;
    token = strtok(bufCpy, " ");
    if (token == nullptr)
    {
        free(bufCpy);
        return (tokens);
    }
    tokens.push_back(token);
    while ((token = strtok(nullptr, ",")) != nullptr)
    {
        if (token[0] == ':')
        {
            std::string tmp;
            tmp = token + 1;
            token = strtok(nullptr, "\r\n");
            if (token != nullptr)
            {
                tmp += token;
                tokens.push_back(tmp);
            }
            free (bufCpy);
            return (tokens);
        }
        else
            tokens.push_back(token);
    }
    free (bufCpy);
    return (tokens);
}
