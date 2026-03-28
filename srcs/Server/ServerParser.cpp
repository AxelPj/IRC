#include "Server.hpp"
#include "IRCNumerics.hpp"
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
        "USER"
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
    std::string buffer = client.getBuffer();
    while (!buffer.empty() && (buffer.back() == '\r' || buffer.back() == '\n'))
        buffer.pop_back();
    std::vector<std::string> tokens = tokenSpace(buffer);
    client.setRemoveBuffer();
    if (tokens.empty() == true)
        return ;
    for (size_t i = 0; i < tokens[0].size(); i++)
        tokens[0][i] = toupper(tokens[0][i]); 
    std::string nick = client.getNick().empty() ? "*" : client.getNick();
    if (tokens.size() == 1 && tokens[0] != "TOPIC" && tokens[0] != "QUIT")
                return ;
    if(client.getRegistered() == false && tokens[0] != "NICK" && tokens[0] != "USER")
    {
        sendMsg(ERR_NOTREGISTERED("server", nick), client.getFd());
        return ;
    }
    for (size_t i = 0; i < tokens.size(); i++)
        std::cout << tokens[i] << std::endl;
    switch(choiceParser(tokens))
    {
        case 0: //NICK
            flag = parserCmdNick(tokens);
            if (flag == -1)
                sendMsg(ERR_NONICKNAMEGIVEN("server", nick), client.getFd());
            else if (flag == -2)
                sendMsg(ERR_ERRONEUSNICKNAME("server", nick), client.getFd());
            else if (flag == -3)
                sendMsg(ERR_NICKNAMEINUSE("server", nick, tokens[1]), client.getFd());
            if (flag == 0)
                cmdNick(client, tokens);
            else if (flag != -1 && flag != -2 && flag != -3)
                sendMsg("Usage: NICK <nickname>, sets your nick\r\n", client.getFd());
            break ;
        case 1: //JOIN
            if (tokens[1].find(',') != std::string::npos)
                flag = parserCmdJoinMulti(tokens, client);
            else
            {
                flag = parserCmdJoin(tokens, client);
                if (flag == 0)
                    cmdJoin(client, tokens[1], false);
                else if (flag == 1)
                    cmdJoin(client, tokens[1], true);
                else if (flag == 2)
                    ;
                else if (flag == -2)
                    sendMsg(ERR_INVITEONLYCHAN("server", client.getNick(), tokens[1]), client.getFd());
                else if (flag == -3)
                    sendMsg(ERR_BADCHANNELKEY("server", client.getNick(), tokens[1]), client.getFd());
				else if (flag == -4)
					sendMsg(ERR_NEEDMOREPARAMS("server", client.getNick(), tokens[0]), client.getFd());
                else if (flag == -1)
                    sendMsg(ERR_CHANNELISFULL("server", client.getNick(), tokens[1]), client.getFd());
				else //Wrong channel name
					sendMsg(ERR_NOSUCHCHANNEL("server", tokens[1]), client.getFd());
            }
            break;
        case 2: //PART
            if (tokens[1].find(',') != std::string::npos)
                flag = parserCmdPartMulti(tokens, client);
            else
            {
                flag = parserCmdPart(tokens, client);
                if (flag == 0)
                    cmdPart(client, tokens, 0); // no reason
                else if (flag == 1)
                    cmdPart(client, tokens, 1); // reason
                else if (flag == -1)
                    sendMsg(ERR_NEEDMOREPARAMS("server", client.getNick(), "PART"), client.getFd());
                else if (flag == -2)
					sendMsg(ERR_NOSUCHCHANNEL("server", tokens[1]), client.getFd());
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
                {
                    if (tokens.size() > 3)
                    {
                        for (size_t i = 2; i < tokens.size(); i++)
                            tokens[2] += tokens[i];
                    }
                    sendMsg(tokens[2], getClient(tokens[1]).getFd());
                }
                else if (flag == 1)
                {
                    if (tokens.size() > 3)
                    {
                        for (size_t i = 2; i < tokens.size(); i++)
                            tokens[2] += tokens[i];
                    }
                    sendMsgChan(tokens[2], *this->_listChannel[tokens[1]], client.getFd());
                }
                else if (flag == -1)
                    sendMsg(ERR_NORECIPIENT("server", client.getNick()), client.getFd());
                else if (flag == -2)
                    sendMsg(ERR_NOTEXTOSEND("server", client.getNick()), client.getFd());
                else if (flag == -3)
                    sendMsg(ERR_NOSUCHNICK("server", tokens[1]), client.getFd());
            }
            break ;
        case 4: //KICK
            if (tokens[1].find(',') != std::string::npos)
                parserCmdKickMulti(tokens, client);
            else
            {
                flag = parserCmdKick(tokens, client);
                if (flag == -1)
                    sendMsg(ERR_NEEDMOREPARAMS("server", client.getNick(), tokens[0]), client.getFd());
                else if (flag == -2) 
                    sendMsg(ERR_NOSUCHCHANNEL("server", tokens[1]), client.getFd());
                else if (flag == -3) 
                    sendMsg(ERR_NOTONCHANNEL("server", client.getNick(), tokens[1]), client.getFd());
                else if (flag == -4)
                    sendMsg(ERR_USERNOTINCHANNEL("server", tokens[2], tokens[1]), client.getFd());
                else if (flag == -5)
                    sendMsg(ERR_CHANOPRIVSNEEDED("server", client.getNick(), tokens[1]), client.getFd());
                if (flag == 0)
                    cmdKick(tokens, getClient(tokens[2]), *this->_listChannel[tokens[1]], false);
                else if (flag == 1)
                    cmdKick(tokens, getClient(tokens[2]), *this->_listChannel[tokens[1]], true);
            }
            break ;
        case 5: //INVITE
            flag = parserCmdInvite(tokens, client);
            if (flag == -1)
                sendMsg(ERR_NOSUCHNICK("server", tokens[1]), client.getFd());
			else if (flag == -2)
				sendMsg(ERR_NOSUCHCHANNEL("server", tokens[2]), client.getFd());
            else if (flag == -3)
				sendMsg(ERR_NOTONCHANNEL("server", client.getNick(), tokens[2]), client.getFd());
            else if (flag == -4)
				sendMsg(ERR_CHANOPRIVSNEEDED("server", client.getNick(), tokens[2]), client.getFd());
			else if (flag == -5)
				sendMsg(ERR_USERONCHANNEL("server", client.getNick(), tokens[1], tokens[2]), client.getFd());
			else
				cmdInvite(client, tokens);
            break ;
        case 6: //TOPIC
            flag = parserCmdTopic(tokens, client);
            if (flag == 0)
                cmdTopic(tokens, client);
			else if (flag == -1)
				sendMsg(ERR_NEEDMOREPARAMS("server", client.getNick(), tokens[0]), client.getFd());
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
                sendMsg(ERR_UNKNOWNMODE("server", client.getNick(), tokens[2]), client.getFd());
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
        case 11: //USER
            flag = parserCmdUser(tokens, client);
            if (flag == 0)
                cmdUser(client, tokens);
            else if (flag == -1)
                sendMsg(ERR_NEEDMOREPARAMS("server", nick, "USER"), client.getFd());
            else if (flag == -2)
                sendMsg(ERR_ALREADYREGISTERED("server", nick), client.getFd());
            else if (flag == -3)
                sendMsg(ERR_INVALIDUSERNAME("server", nick), client.getFd());
            else if (flag == -4)
                sendMsg(ERR_NICKNAMEINUSE("server", nick, tokens[1]), client.getFd());
            break;
        default: //UNKNOWN
            sendMsg(ERR_UNKNOWNCOMMAND("server", tokens[0]), client.getFd());
            break;
    }
}
