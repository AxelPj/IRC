#include "../include/Server.hpp"

Server::Server(int port, std::string password)
{
    if (port < 1024 || port > 65535)
        throw std::invalid_argument("Error Port invalide");
    else
        this->_port = port;
    if (password.empty())
        this->_password = "default";
    else
        this->_password = password;
}

void    Server::init()
{
    // use struct sockaddr_in to bind the socket to the specified port and listen for incoming connections
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(this->_port);
    addr.sin_addr.s_addr = INADDR_ANY;
    int opt = 1;

    // create a socket with AF_INET, SOCK_STREAM, 0,
    // AF_INET is the address family for IPv4, SOCK_STREAM is the type of socket for TCP, and 0 is the protocol (0 is the default protocol)
    this->_sockfd = socket(AF_INET, SOCK_STREAM, 0);
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

pollfd Server::acceptClient(pollfd socketserv)
{
    struct sockaddr_in addr;
    pollfd newSocketclient;
    socklen_t ptrSizestruct = sizeof(addr);
    newSocketclient.fd = accept(socketserv.fd, (sockaddr*)&addr, &ptrSizestruct);
    newSocketclient.events = POLLIN;
    newSocketclient.revents = 0;
    return newSocketclient;
}

int    Server::recvClient(pollfd socketclient)
{
    int ret = recv(socketclient.fd, this->_buffer, 1024, 0);
    int signal;
    if (ret == 0)
        return(0);
    else if (ret == -1);
        //function erreur
    else;
        //envoie au parser qui prendra en parametre le buffer et le pollfd socketclient
        //pour l'envoie au differente fonction
    return (signal);
}

void    Server::run()
{
    //init function poll (struct pollfd)
    struct sockaddr_in addr;
    std::vector<pollfd> socketIrc;
    // maybe variable std::map<Client, pollfd> ?? 
    pollfd socketServ;
    socketServ.fd = this->_sockfd;
    socketServ.events = POLLIN;
    socketServ.revents = 0;
    socketIrc.push_back(socketServ);
    int pollAccept;
    while (1)
    {
        pollAccept = poll(&socketIrc[0], socketIrc.size(), -1);
        for(int i = 0; i < socketIrc.size(); i++)
        {
            // & = binary verification in value POLLIN
            if (socketIrc[i].revents & POLLIN)
            {
                if (i == 0)
                {
                    pollfd ret;
                    ret = acceptClient(socketIrc[0]);
                    if (ret.fd == -1)
                    {
                        std::cerr << "Error: client socket aborts" << std::endl;
                        continue;
                    }
                    socketIrc.push_back(ret);
                    //create client (class)
                    //other
                }
                else
                {
                    int ret = recvClient(socketIrc[i]); //rajouter la classe client en raport au socket fd ?
                    //gerer la deco d'un client?
                    if (ret == 0)
                    {
                        //gestion du fd et du vector
                    }
                    if (ret == -1) //erreur recv
                    {
                        //gestion du fd et du vector
                    }
                }
            }
        }
    }
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

