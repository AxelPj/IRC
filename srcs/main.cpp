#include "../include/Server.hpp"

int main(int ac, char **av)
{
    if (ac < 2 || ac > 3)
    {
        std::cerr << "Usage: " << av[0] << " <port> <password>" << std::endl;
        return (1);
    }
    try
    {
        std::string password = "";
        if (ac == 3)
            password = av[2];
        Server serv(std::stoi(av[1]), password);
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