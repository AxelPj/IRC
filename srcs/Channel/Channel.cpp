#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel() : _userLimit(-1)
{
    for (int i = 0; i < LIST_MODE; i++)
        this->_modList[i] = false;
}

Channel::Channel(const Client &cli, const std::string &name)
    : _name(name), _userLimit(-1)
{
    for (int i = 0; i < LIST_MODE; i++)
        this->_modList[i] = false;
    this->_memberList[&cli] = CONNECTED;
}

Channel::~Channel(){}

// ─── GETTERS ────────────────────────────────────────────────────────────────

const std::string &Channel::getName() const
{
    return (this->_name);
}

const std::string &Channel::getTopic() const
{
    return (this->_topic);
}

const std::string &Channel::getPassword() const
{
    return (this->_password);
}

int Channel::getUserLimit() const
{
    return (this->_userLimit);
}

bool *Channel::getModList()
{
    return (this->_modList);
}

int Channel::getStatusClient(const Client &client) const
{
    std::map<const Client*, ClientStatus>::const_iterator it = this->_memberList.find(&client);
    if (it != this->_memberList.end())
        return (it->second);
    return (-1);
}

std::map<const Client*, ClientStatus> &Channel::getMemberList()
{
    return (this->_memberList);
}

// ─── SETTERS ────────────────────────────────────────────────────────────────

void Channel::setName(const std::string &name)
{
    this->_name = name;
}

void Channel::setTopic(const std::string &topic)
{
    this->_topic = topic;
}

void Channel::setPassword(const std::string &password)
{
    this->_password = password;
}

void Channel::setUserLimit(int limit)
{
    this->_userLimit = limit;
}

void Channel::setStatusClient(const Client &client, ClientStatus status)
{
    this->_memberList[&client] = status;
}


bool Channel::setInviteOnly(bool active)
{
    if (active)
        this->_modList[INVITE_ONLY] = true;
    else
        this->_modList[INVITE_ONLY] = false;
    return (this->_modList[INVITE_ONLY]);
}

bool Channel::setTopicResctriction(bool active)
{
    if(active)
        this->_modList[TOPIC_OPE] = true;
    else
        this->_modList[TOPIC_OPE] = false;
    return (this->_modList[TOPIC_OPE]);
}

// ─── UTILS ──────────────────────────────────────────────────────────────────

bool Channel::isInvited(const Client &client) const
{
    std::map<const Client*, ClientStatus>::const_iterator it = this->_memberList.find(&client);
    if (it != this->_memberList.end())
        return (it->second == INVITED);
    return (false);
}

bool Channel::isMember(const Client &client) const
{
    std::map<const Client*, ClientStatus>::const_iterator it = this->_memberList.find(&client);
    if (it != this->_memberList.end())
        return (it->second == CONNECTED || it->second == OP);
    return (false);
}

bool Channel::isOp(const Client &client) const
{
    std::map<const Client*, ClientStatus>::const_iterator it = this->_memberList.find(&client);
    if (it != this->_memberList.end())
        return (it->second == OP);
    return (false);
}

std::string Channel::namesReply(const Client& client)
{
	std::string namesList = "";
    for (std::map<const Client*, ClientStatus>::iterator it = _memberList.begin(); it != _memberList.end(); it++)
	{
		Client c = *it->first;
		if (it->second != INVITED)
		{
			if (it != _memberList.begin())
				namesList += " ";
			if (isOp(c))
				namesList += "@";
			namesList += c.getNick();
		}
	}
	return RPL_NAMREPLY("server", client.getNick(), _name, namesList);
}

bool *Channel::whichMod()
{
    return (this->_modList);
}

void Channel::removeMember(const Client &client)
{
    for (std::map<const Client*, ClientStatus>::iterator it = _memberList.begin(); it != _memberList.end(); it++)
    {
        if (it->first->getNick() == client.getNick())
        {
            _memberList.erase(it);
            return ;
        }
    }
}

void Channel::invite(Client &user)
{
    this->_memberList[&user] = INVITED;
}

void Channel::join(Client &user)
{
    this->_memberList[&user] = CONNECTED;
}
