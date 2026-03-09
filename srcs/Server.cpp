#include "../include/Server.hpp"

Server::Server(int port, std::string password)
{
    if (port < 1024 || port > 65535)
        throw std::invalid_argument("Error Port invalide");
    else
        this->_port = port;
    if (password.empty())
        this->_password = "default_password";
    else
        this->_password = password;
}

void    Server::init()
{
    // use struct sockaddr_in to bind the socket to the specified port and listen for incoming connections
    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->_port);
    addr.sin_addr.s_addr = INADDR_ANY;
    int opt = 1;

    // create a socket with AF_INET, SOCK_STREAM, 0,
    // AF_INET is the address family for IPv4, SOCK_STREAM is the type of socket for TCP, and 0 is the protocol (0 is the default protocol)
    this->_sockfd = socket (AF_INET, SOCK_STREAM, 0);
    if (this->_sockfd == INVALID_SOCKET)
        throw std::runtime_error("Error with creating socket");
    std::cout << "Socket created" << std::endl;
    // Set socket options at socket level to allow port reuse
    if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("setsockopt failed");
    std::cout << "socket is set" << std::endl;
    //port reservation with the function bind
    if (bind(this->_sockfd, (sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("bind failed");
    else
        std::cout << "Server binded on port " << this->_port << std::endl;
    if (listen(this->_sockfd, 5) < 0)
        throw std::runtime_error("Error : listen failed");
    else
        std::cout << "Server is listening on port " << this->_port << std::endl;
}

void    Server::run()
{

}

Server::~Server()
{
    close(this->_sockfd);
    std::cout << "Server closed" << std::endl;
}

// getters 
int Server::get_port() const
{
    return (this->_port);
}

int Server::get_sockfd() const
{
    return (this->_sockfd);
}

