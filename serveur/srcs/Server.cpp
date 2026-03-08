#include "Server.hpp"

Server::Server(int port, std::string password)
{
    if (port < 1024 || port > 65535)
        throw std::invalid_argument("Port invalide");
    else
        this->_port = port;
    if (password.empty())
        this->_password = void;
    else
        this->_password = password;
    

}

Server::~Server()
{

}