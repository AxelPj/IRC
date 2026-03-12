#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"

class Client
{
    private:
        Client();
        Client(pollfd Socketclient, sockaddr_in addrClient);
};

#endif
