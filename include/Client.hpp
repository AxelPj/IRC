#include "Server.hpp"

class Client
{
    private:
        Client();
        Client(pollfd Socketclient, sockaddr_in addrClient);
};