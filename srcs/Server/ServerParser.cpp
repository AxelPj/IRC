#include "Server.hpp"

std::vector<std::string>  Server::tokenParser(std::string buffer)
{
    char *bufCpy = strcpy(bufCpy, buffer.c_str());
    std::vector<std::string> token;
    int i = 0;

    while (bufCpy)
    {
        token[i] = strtok(bufCpy, " "); 
        i++;
    }
    return (token);
}

bool    Server::choiceParser(std::vector<std::string> token)
{
    std::string cmdString[12] =
    {
        "/NICK",
        "/JOIN",
        "/PART",
        "/PRIVMSG",
        "/KICK",
        "/INVITE",
        "/TOPIC",
        "/MODE",
        "/QUIT",
        "/PING",
        "/PONG",
        "/UNKNOWN"
    };
    void (Server::*cmdsFunction[11])() = 
    {
        Server::cmdNick(token),
        Server::cmdJoin(token),
        Server::cmdPart(token),
        Server::cmdPrivMsg(token),
        Server::cmdKick(token),
        Server::cmdInvite(token),
        Server::cmdTopic(token),
        Server::cmdMode(token),
        Server::cmdQuit(token),
        Server::cmdPing(token),
        Server::cmdPong(token),
    };
    for(int i = 0; i < 12; i++)
    {
        if (token[0] == cmdString[i])
        {
            int index = i;
            switch (index)
            {
                case 0:
                    return (this->*cmdsFunction[0])();
                case 1:
                    return (this->*cmdsFunction[1])();
                case 2:
                    return (this->*cmdsFunction[2])();              
                case 3:
                    return(this->*cmdsFunction[3])();
                case 4:
                    return(this->*cmdsFunction[4])();
                case 5:
                    return (this->*cmdsFunction[5])();
                case 6:
                    return (this->*cmdsFunction[6])();
                case 7:
                    return (this->*cmdsFunction[7])();
                case 8:
                    return (this->*cmdsFunction[8])();
                case 9:
                    return (this->*cmdsFunction[9])();
                case 10:
                    return (this->*cmdsFunction[10])();
                case 11:
                    return (this->*cmdsFunction[11])();
                default:
                    return (-1); 
                    break;
            }
        }
    }
}

void    Server::processParser(Client &client)
{
    std::vector<std::string> token = tokenParser(client.getBuffer());
    if (token[0][0] == '/')
    {
        token[0] = toupper(token[0]);
        choiceParser(token);
    }
};
