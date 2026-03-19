#include "Server.hpp"

int Server::checkChannelExist(std::string channelName)
{
    if (this->_listChannel.find(channelName) != this->_listChannel.end())
        return (0);
    else
        return (-1);
}

std::vector<std::string>  Server::tokenComma(std::string buffer)
{
    char *bufCpy = strdup(buffer.c_str());
    std::vector<std::string> tokens;

    char *token;
    token = strtok(bufCpy, " ");
    if (token == nullptr)
    {
        free(bufCpy);
        return (tokens);
    }
    tokens.push_back(token);
    while ((token = strtok(nullptr, ",")) != nullptr)
    {
        if (token[0] == ':')
        {
            std::string tmp;
            tmp = token + 1;
            token = strtok(nullptr, "\r\n");
            if (token != nullptr)
            {
                tmp += token;
                tokens.push_back(tmp);
            }
            free (bufCpy);
            return (tokens);
        }
        else
            tokens.push_back(token);
    }
    free (bufCpy);
    return (tokens);
}

std::vector<std::string>  Server::tokenSpace(std::string buffer)
{
    char *bufCpy = strdup(buffer.c_str());
    std::vector<std::string> tokens;

    char *token;
    token = strtok(bufCpy, " ");
    if (token == nullptr)
    {
        free(bufCpy);
        return (tokens);
    }
    tokens.push_back(token);
    while ((token = strtok(nullptr, " ")) != nullptr)
    {
        if (token[0] == ':')
        {
            std::string tmp;
            tmp = token + 1;
            token = strtok(nullptr, "\r\n");
            if (token != nullptr)
            {
                tmp += token;
                tokens.push_back(tmp);

            }
            free(bufCpy);
            return(tokens);
        }
        else
            tokens.push_back(token);
    }
    free(bufCpy);
    return (tokens);
}