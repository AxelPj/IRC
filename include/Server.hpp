#include <iostream>
#include <list>
#include <map>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

class Server
{
    private:
        Server();
        struct sockaddr_in addr;
        int     _port;
        int     _sockfd;
        /*         std::map<enum idChall, std::string nameChannel> _channels;
        std::map<int idUser, std::string username> _users; */
        std::string _password;
        void quit();
        /*         void ping(Client user);
        */        
   public:
        Server(int port, std::string password);
        ~Server();
        void    init();
        void    run();
        void    addToChannel();
        void    addChannel(std::string channel);
        void    removeChannel(std::string channel);

        // getters
        int     get_port() const;
        int     get_sockfd() const;
/*         void    sendMessage(std::string message, Client user);
 */};


