#include "../include/Server.hpp"

int main(int ac, char **av)
{
    if (ac != 3)
    {
        std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
        return (1);
    }
    try
    {
		//stoi is not part of C++98, this needs to be changed at some point
        Server serv(std::stoi(av[1]), av[2]);
        serv.init();
        serv.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return (1);
    }
    return (0);
}
