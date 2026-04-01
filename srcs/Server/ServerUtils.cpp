#include "Server.hpp"

bool Server::channelExists(const std::string &channelName)
{
	return (this->_listChannel.find(channelName) != this->_listChannel.end());
}

bool Server::userExists(const std::string &userName)
{
	for (std::map<int, Client *>::iterator it = this->_listClient.begin(); it != this->_listClient.end(); it++)
	{
		if (it->second->getNick() == userName)
			return (true);
	}
	return (false);
}

std::vector<std::string> Server::tokenComma(const std::string &buffer)
{
	char *bufCpy = strdup(buffer.c_str());
	std::vector<std::string> tokens;
	char *token;
	token = strtok(bufCpy, ",");
	tokens.push_back(token);
	while ((token = strtok(NULL, ",\r\n")) != NULL)
		tokens.push_back(token);
	free(bufCpy);
	return (tokens);
}

std::vector<std::string> Server::tokenSpace(const std::string &buffer)
{
	char *bufCpy = strdup(buffer.c_str());
	std::vector<std::string> tokens;
	char *token;
	token = strtok(bufCpy, " \r\n");
	if (token == NULL)
	{
		free(bufCpy);
		return (tokens);
	}
	tokens.push_back(token);
	while ((token = strtok(NULL, " \r\n")) != NULL)
	{
		if (token[0] == ':')
		{
			std::string tmp = token + 1;
			tokens.push_back(tmp);
			free(bufCpy);
			return (tokens);
		}
		else
			tokens.push_back(token);
	}
	free(bufCpy);
	return (tokens);
}

void Server::createChannel(const std::string &channelName, const Client *client)
{
	Channel *newChannel = new Channel(*client, channelName);
	this->_listChannel[channelName] = newChannel;
}

void Server::addMode(Channel &channel, char mode, const std::string &param)
{
	if (mode == 'i')
		channel.setInviteOnly(true);
	else if (mode == 't')
		channel.setTopicRestriction(true);
	else if (mode == 'k')
		channel.setPassword(param);
	else if (mode == 'l')
		channel.setUserLimit(atoi(param.c_str()));
	else if (mode == 'o')
		channel.setStatusClient(getClient(param), OP);
}

void Server::removeMode(Channel &channel, char mode, const std::string &param)
{
	if (mode == 'i')
		channel.setInviteOnly(false);
	else if (mode == 't')
		channel.setTopicRestriction(false);
	else if (mode == 'k')
		channel.setPassword("");
	else if (mode == 'l')
		channel.setUserLimit(0);
	else if (mode == 'o')
		channel.setStatusClient(getClient(param), CONNECTED);
}

void Server::sendMsg(const std::string &msg, const Client &client)
{
	int ret;
	int socket = client.getFd();
	const std::string &ipHost = client.getAddress();

	const char *buf = msg.c_str();
	while (strlen(buf) != 0)
	{
		ret = send(socket, buf, strlen(buf), 0);
		if (ret == -1)
		{
			std::cerr << "Error: impossible send message to client (" << ipHost << ")";
			break;
		}
		else if (ret == 0)
			break;
		else
			buf += ret;
	}
}

void Server::sendMsgChan(const std::string &msg, Channel &channel, int senderFd)
{
	std::map<const Client *, ClientStatus> &members = channel.getMemberList();

	for (std::map<const Client *, ClientStatus>::const_iterator i = members.begin(); i != members.end(); ++i)
	{
		const Client *client = i->first;
		ClientStatus status = i->second;

		if (status != CONNECTED && status != OP)
			continue;
		if (client->getFd() == senderFd)
			continue;
		sendMsg(msg, *client);
	}
}

void Server::broadcastMsg(const std::string &msg)
{
	for (std::map<int, Client*>::const_iterator i = _listClient.begin(); i != _listClient.end(); ++i)
		sendMsg(msg, *i->second);
}

int Server::findChanForKick(const std::vector<std::string> &tokens)
{
	for (size_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i][0] == '#')
		{
			if (tokens[i + 1].empty() == false)
				return (i);
			return (-1);
		}
	}
	return (-1);
}
