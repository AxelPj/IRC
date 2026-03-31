#include "Channel.hpp"
#include "Client.hpp"

Channel::Channel() : _userLimit(-1)
{
    for (int i = 0; i < LIST_MODE; i++)
		this->_modList[i] = false;
	this->_creationTime = std::time(NULL);
}

Channel::Channel(const Client &cli, const std::string &name)
    : _name(name), _userLimit(-1)
{
    for (int i = 0; i < LIST_MODE; i++)
        this->_modList[i] = false;
    this->_memberList[&cli] = OP;
	this->_creationTime = std::time(NULL);
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

std::string Channel::getCreationTimeAsString() const
{
	std::string res;
	std::stringstream ss(res);
	ss << this->_creationTime;
	ss >> res;
	return (res);
}

std::string Channel::getModeString() const
{
	std::string res = "+";
	if (_modList[INVITE_ONLY])
		res += "i";
	if (_modList[PASSWORD])
		res += "k";
	if (_modList[LIMIT])
		res += "l";
	if (_modList[TOPIC_OPE])
		res += "t";

	//Mode args
	if (_modList[LIMIT])
	{
		std::ostringstream s;
		s << _userLimit;
		res += " " + s.str() + " ";
	}
	return (res);
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
	if (password.empty())
		this->_modList[PASSWORD] = false;
	else 
    	this->_modList[PASSWORD] = true;
}

void Channel::setUserLimit(int limit)
{
    this->_userLimit = limit;
    this->_modList[LIMIT] = (limit > 0);
}

void Channel::setStatusClient(const Client &client, ClientStatus status)
{
    this->_memberList[&client] = status;
}


bool Channel::setInviteOnly(bool active)
{
	this->_modList[INVITE_ONLY] = active;
    return (this->_modList[INVITE_ONLY]);
}

bool Channel::setTopicRestriction(bool active)
{
	this->_modList[TOPIC_OPE] = active;
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
		if (it->second != INVITED)
		{
			if (!namesList.empty())
				namesList += " ";
			if (it->second == OP)
				namesList += "@";
			namesList += it->first->getNick();
		}
	}
	return RPL_NAMREPLY(SERVER_NAME, client.getNick(), _name, namesList);
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

bool    Channel::isEmpty()
{
    if (this->_memberList.empty())
        return(true);
    return(false);
}
