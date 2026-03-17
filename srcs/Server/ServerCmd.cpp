#include "Server.hpp"

int Server::cmdNick(std::vector<std::string> token)
{
    if (token.size() != 2) {
        std::cerr << "Error: NICK command requires an argument." << std::endl;
        return -1;
    }
    std::string nickname = token[1];
    for (size_t i = 0, i < _listclient.size(); i++)
    {
        if (this->_listclient[i].getNickname() == nickname)
        {
            std::cerr << "Error: Nickname already in use." << std::endl;
            return -1;
        }
    }

}