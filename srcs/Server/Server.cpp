#include "Server.hpp"
#include <cerrno>

Server::Server(int port, const std::string& password)
{
    if (port < 1024 || port > 65535)
        throw std::invalid_argument("Error: invalid port");
    else
        this->_port = port;
    if (password.empty())
        this->_password = "";
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
        throw std::runtime_error("Error: with creating socket");
    std::cout << "Socket created" << std::endl;
    // Set socket options at socket level to allow port reuse
    if (setsockopt(this->_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        throw std::runtime_error("setsockopt failed");
    std::cout << "socket is set" << std::endl;
    //port reservation with the function bind
    if (bind(this->_sockfd, (sockaddr*)&addr, sizeof(addr)) < 0)
        throw std::runtime_error("Error: bind failed");
    else
        std::cout << "Server bound on port " << this->_port << std::endl;
    if (listen(this->_sockfd, 5) < 0)
        throw std::runtime_error("Error: listen failed");
    else
        std::cout << "Server is listening on port " << this->_port << std::endl;
}

Client* Server::acceptClient(sockaddr_in *addr, pollfd *newSocketclient)
{
    socklen_t ptrSizestruct = sizeof(*addr);
    newSocketclient->fd = accept(this->_socketIrc[0].fd, (sockaddr*)addr, &ptrSizestruct);
    if (newSocketclient->fd < 0)
        return(NULL);
    newSocketclient->events = POLLIN;
    newSocketclient->revents = 0;
    return (new Client(*addr, *newSocketclient));
}

bool    Server::recvClient(const pollfd &socketclient, Client &client)
{
    int ret = recv(socketclient.fd, this->_buffer, 1024, 0);
    if (ret == 0)
    {
        std::cerr << "[RECV] client disconnected fd=" << socketclient.fd << std::endl;
        return (false);
    }
    if (ret < 0)
        return (false);
    this->_buffer[ret] = '\0';
    client.setAddBuffer(this->_buffer);
    return (true);
}

void Server::removeClient(int fdClient, int i)
{
    Client *client = _listClient[fdClient];
    for (std::map<std::string, Channel*>::iterator it = _listChannel.begin(); it != _listChannel.end();)
    {
        if (it->second->isMember(*client))
        {
            sendMsgChan(MSG_QUIT(client->getNick(), client->getUser(), client->getAddress(), "Connection lost"), *it->second, client->getFd());
            it->second->removeMember(*client);
            if (it->second->isEmpty())
            {
                delete it->second;
                it = _listChannel.erase(it);
            }
            else
                it++;
        }
        else
            it++;
    }
    close(fdClient);
    _socketIrc.erase(_socketIrc.begin() + i);
    delete _listClient[fdClient];
    _listClient.erase(fdClient);
}

void    Server::run()
{
    //init function poll (struct pollfd)
    //struct sockaddr_in addr;
    pollfd socketServ;
    socketServ.fd = this->_sockfd;
    socketServ.events = POLLIN;
    socketServ.revents = 0;
    _socketIrc.push_back(socketServ);
    int pollAccept;
    //time_t now = time(nullptr);
    while (1)
    {
        pollAccept = poll(this->_socketIrc.data(), this->_socketIrc.size(), -1);
        if (pollAccept == -1)
		{
            throw std::runtime_error("Error: poll for accept");
			break;
		}
        for(unsigned long int i = 0; i < this->_socketIrc.size(); i++)
        {
            // & = binary verification in value POLLIN
            if (this->_socketIrc[i].revents & POLLIN)
            {
                if (this->_socketIrc[i].fd == this->_sockfd)
                {
                    // if socket listen is that of the Server, call acceptClient() for create NewClient and fill data map and vector
                    pollfd newSocketClient;
                    struct sockaddr_in addr;
                    Client *newClient = acceptClient(&addr, &newSocketClient);
                    if (!newClient)
                    {
                        std::cerr << "Error: client socket aborts" << std::endl;
                        continue ;
                    }
                    std::cerr << "[POLL] accepted client fd=" << newSocketClient.fd << std::endl;
                    this->_socketIrc.push_back(newSocketClient);
                    this->_listClient[newSocketClient.fd] = newClient;
                }
                //if socket listen is that of the client, call recvclient() for fill the buffer
                //and call the parser for redistribute at the function commande Client and receve bool to know deconnect client so call removeClient() 
                else
                {
                    if (recvClient(this->_socketIrc[i], *this->_listClient[this->_socketIrc[i].fd]) == false)
                    {
                        removeClient(this->_socketIrc[i].fd, i);
                        i--;
                    }
                    else 
                    {
                        std::cerr << "[POLL] processing client fd=" << this->_socketIrc[i].fd << std::endl;
                        processParser(*this->_listClient[this->_socketIrc[i].fd]);
                        
                    }
            
                }
            }
            else if (this->_socketIrc[i].revents & POLLERR)
            {
                sendMsg("Error : event POLLERR\r\n", *_listClient[this->_socketIrc[i].fd]);
                removeClient(this->_socketIrc[i].fd, i);
                i--; 
            }
            else if ((this->_socketIrc[i].revents & POLLHUP) || (this->_socketIrc[i].revents & POLLNVAL))
            {
                removeClient(this->_socketIrc[i].fd, i);
                i--;
            }
        }
    }
}

Server::~Server()
{
    for (std::map<int, Client*>::iterator it = _listClient.begin(); it != _listClient.end(); ++it)
        delete it->second;
    for (std::map<std::string, Channel*>::iterator it = _listChannel.begin(); it != _listChannel.end(); it++)
        delete it->second;
    close(this->_socketIrc[0].fd);
    std::cout << "Server closed, goodbye\n" << std::endl;
}

// getters 
int Server::getPort() const
{
    return (this->_port);
}

int Server::getSockfd() const
{
    return (this->_sockfd);
}

char   *Server::getBuffer()
{
    return(this->_buffer);
}

pollfd Server::getpollfd(int i) const
{
    return(this->_socketIrc[i]);
}

Client& Server::getClient(const std::string &nameClient)
{
    for (std::map<int, Client*>::iterator it = this->_listClient.begin(); it != this->_listClient.end(); it++)
    {
        if (it->second->getNick() == nameClient)
            return (*it->second);
    }
    throw std::runtime_error("Client not found");
}

Channel&    Server::getChannel(const std::string &name)
{
    return(*this->_listChannel[name]);
}
