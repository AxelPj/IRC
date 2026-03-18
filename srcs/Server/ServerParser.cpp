#include "Server.hpp"

std::vector<std::string>  Server::tokenParser(std::string buffer)
{
    char *bufCpy = strcpy(bufCpy, buffer.c_str());
    std::vector<std::string> tokens;

    char *token;
    token = strtok(bufCpy, " ");
    if (token == nullptr)
    {
        return ;
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
            return(tokens);
        }
        else
            tokens.push_back(token);
    }
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
                flag = parserCmdJoin(tokens);
                if (flag == 0)
                    //cmdJoin(client, tokens);
                else 
                    sendMsg("Usage: JOIN <channel>", client.getfd());
                break ;
            case 2:
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

int parserCmdJoin(std::vector<std::string> tokens)
{
    if (checkChannel(tokens[1]))
            return(1);
    else 

    //verif channel a des mods
}

int parserCmdPart(std::vector<std::string> tokens)
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

int parserCmdPrivMsg(std::vector<std::string> tokens)
{
    //creation d un channel private si targer == client sinon chan normal si target == chan
    if (checkChannel(tokens[1]))
        return(1);
    else if ()
}

int parserCmdKick(std::vector<std::string> tokens)
{
    //verif que le client est op + chan existe + client a kick dans le chan
}

int parserCmdInvite(std::vector<std::string> tokens)
{
    //verif que le client est op + chan existe + client a inviter dans le chan
}

int parserCmdTopic(std::vector<std::string> tokens)
{
 //verif mode + droits op + client op si droit op
}

int parserCmdMode(std::vector<std::string> tokens)
{
    //verif client est op + mode valide
}

int parserCmdPing(std::vector<std::string> tokens)
{

}

int parserCmdPong(std::vector<std::string> tokens)
{

}

int parserCmdQuit(std::vector<std::string> tokens)
{

}

int parserCmdReconnect(std::vector<std::string> tokens)
{

}

int Server::checkChannelExist(std::string channelName)
{
    std::map<std::string,Channel>::iterator it;
    if (this->_listchannel.find(channelName) != this->listchannel.end())
        return (0);
    else
        return(-1);
}
