#include "Server.hpp"
#include "IRCNumerics.hpp"
int    Server::choiceParser(const std::vector<std::string> &tokens)
{
    std::string cmdString[14] =
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
        "USER",
		"WHO",
		"PASS"
    };

    for(int i = 0; i < 14; i++)
    {
        if (tokens[0] == cmdString[i])
            return (i);
    }
    return(-1);
}

void    Server::processParser(Client &client)
{
    std::string buffer = client.getBuffer();
    size_t pos = 0;
    int flag;

    while ((pos = buffer.find("\r\n")) != std::string::npos)
    {
        std::string line = buffer.substr(0, pos);
        if (!line.empty())
        {
            std::string processBuffer = line;
            while (!processBuffer.empty() && (*processBuffer.rbegin() == '\r' || *processBuffer.rbegin() == '\n'))
                processBuffer.erase(processBuffer.size() - 1);
            std::vector<std::string> tokens = tokenSpace(processBuffer);
            if (!tokens.empty())
            {
				for (size_t i = 0; i < tokens.size(); i++)
					std::cout << tokens[i] << std::endl;
                for (size_t i = 0; i < tokens[0].size(); i++)
                    tokens[0][i] = toupper(tokens[0][i]);
                std::string nick = client.getNick().empty() ? "*" : client.getNick();
                if (!(tokens.size() == 1 && tokens[0] != "TOPIC" && tokens[0] != "QUIT"))
                {
                    if(!client.getRegistered() && tokens[0] != "NICK" && tokens[0] != "USER" && tokens[0] != "CAP" && tokens[0] != "PASS")
						sendMsg(ERR_NOTREGISTERED(SERVER_NAME, nick), client);
                    else if (!client.isAuth() && (tokens[0] == "NICK" || tokens[0] == "USER"))
                    {
						sendMsg(std::string("ERROR: ") + SERVER_NAME + " (Bad Password)\r\n", client);
						removeClient(client);
						return;
					}
					else
					{
                        switch(choiceParser(tokens))
                        {
                            case 0: //NICK
                                flag = parserCmdNick(tokens);
                                if (flag == 0)
                                    cmdNick(client, tokens);
                                else if (flag == -1)
                                    sendMsg(ERR_NONICKNAMEGIVEN(SERVER_NAME, nick), client);
                                else if (flag == -2)
                                    sendMsg(ERR_ERRONEUSNICKNAME(SERVER_NAME, nick), client);
                                else if (flag == -3)
                                    sendMsg(ERR_NICKNAMEINUSE(SERVER_NAME, nick, tokens[1]), client);
                                break ;
                            case 1: //JOIN
                                if (tokens[1].find(',') != std::string::npos)
                                    parserCmdJoinMulti(tokens, client);
                                else
                                {
                                    flag = parserCmdJoin(tokens, client);
                                    if (flag == 0)
                                        cmdJoin(client, tokens[1], false);
                                    else if (flag == 1)
                                        cmdJoin(client, tokens[1], true);
									else if (flag == 2)
                                        cmdPartMulti(client, std::vector<std::string>(), std::string(), true);
                                    else if (flag == -2)
                                        sendMsg(ERR_INVITEONLYCHAN(SERVER_NAME, client.getNick(), tokens[1]), client);
                                    else if (flag == -3)
                                        sendMsg(ERR_BADCHANNELKEY(SERVER_NAME, client.getNick(), tokens[1]), client);
                                    else if (flag == -4)
                                        sendMsg(ERR_NEEDMOREPARAMS(SERVER_NAME, client.getNick(), tokens[0]), client);
                                    else if (flag == -5)
                                        sendMsg(ERR_BADCHANMASK(SERVER_NAME, tokens[1]), client);
                                    else if (flag == -1)
                                        sendMsg(ERR_CHANNELISFULL(SERVER_NAME, client.getNick(), tokens[1]), client);
                                }
                                break;
                            case 2: //PART
                                if (tokens[1].find(',') != std::string::npos)
                                {
                                        std::vector<std::string> chanVal = parserCmdPartMulti(tokens, client);
                                        if (chanVal.empty() == true)
                                            break ;
                                        else
                                        {
                                            if(tokens.empty() == true)
                                                cmdPartMulti(client, chanVal, "", false);
                                            else
                                                cmdPartMulti(client, chanVal, tokens[0], false);
                                        }
                                }   
                                else
                                {
                                    flag = parserCmdPart(tokens, client);
                                    if (flag == 0)
                                        cmdPart(client, tokens, 0); // no reason
                                    else if (flag == 1)
                                        cmdPart(client, tokens, 1); // reason
                                    else if (flag == -1)
                                        sendMsg(ERR_NEEDMOREPARAMS(SERVER_NAME, client.getNick(), "PART"), client);
                                    else if (flag == -2)
                                        sendMsg(ERR_NOSUCHCHANNEL(SERVER_NAME, tokens[1]), client);
                                    else if (flag == -3)
                                        sendMsg(ERR_NOTONCHANNEL(SERVER_NAME, client.getNick(), tokens[1]), client);
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
                                        sendMsg(MSG_PRIVMSG(client.getNick(), client.getUser(), client.getAddress(), tokens[1], tokens[2]), getClient(tokens[1]));
                                    }
                                    else if (flag == 1)
                                    {
                                        if (tokens.size() > 3)
                                        {
                                            for (size_t i = 2; i < tokens.size(); i++)
                                                tokens[2] += tokens[i];
                                        }
                                        sendMsgChan(MSG_PRIVMSG(client.getNick(), client.getUser(), client.getAddress(), tokens[1], tokens[2]), *this->_listChannel[tokens[1]], client.getFd());
                                    }
                                    else if (flag == -1)
                                        sendMsg(ERR_NORECIPIENT(SERVER_NAME, client.getNick()), client);
                                    else if (flag == -2)
                                        sendMsg(ERR_NOTEXTOSEND(SERVER_NAME, client.getNick()), client);
                                    else if (flag == -3)
                                        sendMsg(ERR_NOSUCHNICK(SERVER_NAME, tokens[1]), client);
                                }
                                break ;
                            case 4: //KICK
                                {
                                    int chanval = findChanForKick(tokens);
                                    if (chanval != -1)
                                    {
                                        std::vector<std::string> tokenSub(tokens.begin() + chanval, tokens.end());
                                        if (tokenSub.size() < 2)
                                        {
                                            std::cout << "je suis la numeros 1\n\n\n";
                                            sendMsg(ERR_NEEDMOREPARAMS(SERVER_NAME, client.getNick(), tokens[0]), client);
                                            break;
                                        }
                                        if (tokenSub[0].find(',') != std::string::npos && tokenSub[1].find(',') != std::string::npos)
                                            parserCmdKickMulti(tokenSub, client, true);
                                        else if (tokenSub[0].find(',') != std::string::npos && tokenSub[1].find(',') == std::string::npos)
                                            parserCmdKickMulti(tokenSub, client, false);
                                        else if (tokenSub[0].find(',') == std::string::npos && tokenSub[1].find(',') != std::string::npos)
                                        {
                                            std::cout << "je suis la numeros 2\n\n\n";
                                            sendMsg(ERR_NEEDMOREPARAMS(SERVER_NAME, client.getNick(), tokens[0]), client);
                                        }
                                        else
                                        {
                                            flag = parserCmdKick(tokenSub, client);
                                            if (flag == -1)
                                            {  
                                                std::cout << "je suis la numeros 3\n\n\n";
                                                sendMsg(ERR_NEEDMOREPARAMS(SERVER_NAME, client.getNick(), tokens[0]), client);
                                            }
                                            else if (flag == -2)
                                                sendMsg(ERR_NOSUCHCHANNEL(SERVER_NAME, tokenSub[0]), client);
                                            else if (flag == -3)
                                                sendMsg(ERR_NOTONCHANNEL(SERVER_NAME, client.getNick(), tokenSub[0]), client);
                                            else if (flag == -4)
                                                sendMsg(ERR_USERNOTINCHANNEL(SERVER_NAME, tokenSub[1], tokenSub[0]), client);
                                            else if (flag == -5)
                                                sendMsg(ERR_CHANOPRIVSNEEDED(SERVER_NAME, client.getNick(), tokenSub[0]), client);
                                            if (flag == 0)
                                                cmdKick(client, tokenSub, getClient(tokenSub[1]), *this->_listChannel[tokenSub[0]], false);
                                            else if (flag == 1)
                                                cmdKick(client, tokenSub, getClient(tokenSub[1]), *this->_listChannel[tokenSub[0]], true);
                                        }
                                    }
                                    else
                                    {
                                                                                    std::cout << "je suis pas pas la numeros 1\n\n\n";
                                        sendMsg(ERR_NEEDMOREPARAMS(SERVER_NAME, client.getNick(), tokens[0]), client);
                                    }
                                }
                                break ;
                            case 5: //INVITE
                                flag = parserCmdInvite(tokens, client);
								std::cout << "FLAGS OK" << std::endl;
								if (flag == -1)
									sendMsg(ERR_NEEDMOREPARAMS(SERVER_NAME, client.getNick(), tokens[0]), client);
								else if (flag == -2)
                                    sendMsg(ERR_NOSUCHNICK(SERVER_NAME, tokens[1]), client);
                                else if (flag == -3)
                                    sendMsg(ERR_NOSUCHCHANNEL(SERVER_NAME, tokens[2]), client);
                                else if (flag == -4)
                                    sendMsg(ERR_NOTONCHANNEL(SERVER_NAME, client.getNick(), tokens[2]), client);
                                else if (flag == -5)
                                    sendMsg(ERR_CHANOPRIVSNEEDED(SERVER_NAME, client.getNick(), tokens[2]), client);
                                else if (flag == -6)
                                    sendMsg(ERR_USERONCHANNEL(SERVER_NAME, client.getNick(), tokens[1], tokens[2]), client);
                                else
                                    cmdInvite(client, tokens);
                                break ;
                            case 6: //TOPIC
                                flag = parserCmdTopic(tokens, client);
                                if (flag == 0)
                                    cmdTopic(tokens, client);
                                else if (flag == -1)
                                    sendMsg(ERR_NEEDMOREPARAMS(SERVER_NAME, client.getNick(), tokens[0]), client);
                                else if (flag == -2)
                                    sendMsg(ERR_NOSUCHCHANNEL(SERVER_NAME, tokens[1]), client);
                                else if (flag == -3)
                                    sendMsg(ERR_CHANOPRIVSNEEDED(SERVER_NAME, client.getNick(), tokens[1]), client);
                                break ;
                            case 7: //MODE
                                flag = parserCmdMode(tokens, client);
                                if (flag == 0)
									cmdModeChannel(client, tokens);
                                else if (flag == -1)
                                    sendMsg(ERR_NEEDMOREPARAMS(SERVER_NAME, client.getNick(), tokens[0]), client);
                                else if (flag == -2)
                                    sendMsg(ERR_NOSUCHCHANNEL(SERVER_NAME, tokens[1]), client);
                                else if (flag == -3)
                                    sendMsg(ERR_CHANOPRIVSNEEDED(SERVER_NAME, client.getNick(), tokens[1]), client);
                                else if (flag == -4)
                                    sendMsg(ERR_UNKNOWNMODE(SERVER_NAME, client.getNick(), tokens[2]), client);
                                else if (flag == -5)
                                    sendMsg(ERR_INVALIDMODEPARAM(SERVER_NAME), client);
								else if (flag == -6)
									sendMsg(ERR_KEYSET(SERVER_NAME, client.getNick(), tokens[1]), client);
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
                                    sendMsg(ERR_NEEDMOREPARAMS(SERVER_NAME, nick, "USER"), client);
                                else if (flag == -2)
                                    sendMsg(ERR_ALREADYREGISTERED(SERVER_NAME, nick), client);
                                else if (flag == -3)
                                    sendMsg(ERR_INVALIDUSERNAME(SERVER_NAME, nick), client);
                                break;
							case 12: //WHO
								flag = parserCmdWho(tokens);
								if (flag == 0)
									cmdWhoUser(client, tokens[1]);
								else if (flag == 1)
									cmdWhoChannel(client, tokens[1]);
								else if (flag == -1)
                                    sendMsg(ERR_NEEDMOREPARAMS(SERVER_NAME, nick, "WHO"), client);
								else if (flag == -2)
									sendMsg(RPL_ENDOFWHO(SERVER_NAME, nick, tokens[1]), client);
								break;
							case 13: //PASS
								if (tokens.size() < 2)
									sendMsg(ERR_NEEDMOREPARAMS(SERVER_NAME, nick, tokens[1]), client);
								else if (client.getRegistered())
									sendMsg(ERR_ALREADYREGISTERED(SERVER_NAME, nick), client);
								else
									cmdPass(client, tokens[1]);
                            default: //UNKNOWN
                                sendMsg(ERR_UNKNOWNCOMMAND(SERVER_NAME, tokens[0]), client);
                                break;
                        }
                    }
                }
            }
        }
        // Remove the processed line and line ending
        if (buffer[pos] == '\r' && pos + 1 < buffer.length() && buffer[pos + 1] == '\n')
            buffer = buffer.substr(pos + 2);  // Remove \r\n
        else
            buffer = buffer.substr(pos + 1);  // Remove \n or \r
    }
    client.setBuffer(buffer);
}
