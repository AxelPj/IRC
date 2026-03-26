#include "Channel.hpp"

Channel::Channel() : _userLimit(-1)
{
    for (int i = 0; i < LIST_MODE; i++)
        this->_modList[i] = false;
}

Channel::Channel(Client &cli, const std::string &name)
    : _name(name), _userLimit(-1)
{
    for (int i = 0; i < LIST_MODE; i++)
        this->_modList[i] = false;
    this->_memberList[&cli] = CONNECTED;
}

Channel::~Channel()
{
}

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
    return (NOT_CONNECTED);
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

void Channel::setStatusClient(Client &client, ClientStatus status)
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

bool Channel::isInvited(Client &client) const
{
    std::map<const Client*, ClientStatus>::const_iterator it = this->_memberList.find(&client);
    if (it != this->_memberList.end())
        return (it->second == INVITED);
    return (false);
}

bool Channel::isMember(Client &client) const
{
    std::map<const Client*, ClientStatus>::const_iterator it = this->_memberList.find(&client);
    if (it != this->_memberList.end())
        return (it->second == CONNECTED || it->second == OP);
    return (false);
}

bool Channel::isOp(Client &client) const
{
    std::map<const Client*, ClientStatus>::const_iterator it = this->_memberList.find(&client);
    if (it != this->_memberList.end())
        return (it->second == OP);
    return (false);
}

bool *Channel::whichMod()
{
    return (this->_modList);
}

// ─── ACTIONS ────────────────────────────────────────────────────────────────

void Channel::join(Client &user, const std::string &password)
{
    // ERR_BANNEDFROMCHAN
    if (this->_memberList.count(&user) && this->_memberList[&user] == BANNED)
        return ;
    // ERR_INVITEONLYCHAN
    if (this->_modList[INVITE_ONLY] && !isInvited(user))
        return ;
    // ERR_CHANNELISFULL
    if (this->_userLimit >= 0 && (int)this->_memberList.size() >= this->_userLimit)
        return ;
    // ERR_BADCHANNELKEY
    if (this->_modList[PASSWORD] && password != this->_password)
        return ;
    this->_memberList[&user] = CONNECTED;
    // JOIN MESSAGE
    // RPL_TOPIC
    // RPL_TOPICTIME
    // RPL_NAMREPLY
    // RPL_ENDOFNAMES
}

void Channel::part(Client &user)
{
    if (this->_memberList.count(&user))
    {
        this->_memberList.erase(&user);
        // PART MESSAGE
    }
    // else ERR_NOTONCHANNEL
}

void Channel::topic(Client &user)
{
    // if topic    → RPL_TOPIC + RPL_TOPICWHOTIME
    // else        → RPL_NOTOPIC
    (void)user;
}

void Channel::topic(Client &user, const std::string &newTopic)
{
    if (!this->_memberList.count(&user))
        return ; // ERR_NOTONCHANNEL
    if (this->_modList[TOPIC_OPE] && !isOp(user))
        return ; // ERR_CHANOPRIVSNEEDED
    this->_topic = newTopic;
    // TOPIC MESSAGE
}

void Channel::names(Client &user)
{
    // secret channel → RPL_ENDOFNAMES only if not member
    if (!isMember(user))
        return ;
    // iterate over memberList
    // RPL_NAMREPLY + RPL_ENDOFNAMES
}

void Channel::kick(Client &issuer, Client &target)
{
    if (!this->_memberList.count(&issuer))
        return ; // ERR_NOTONCHANNEL
    if (!isOp(issuer))
        return ; // ERR_CHANOPRIVSNEEDED
    if (!this->_memberList.count(&target))
        return ; // ERR_USERNOTINCHANNEL
    this->_memberList.erase(&target);
    // KICK MESSAGE
}

void Channel::mode(Client &user, const std::string &modes)
{
    if (!this->_memberList.count(&user))
        return ; // ERR_NOTONCHANNEL
    if (!isOp(user))
        return ; // ERR_CHANOPRIVSNEEDED
    if (modes.empty())
        return ;
    // TODO: parser les modes +/- i k o l t
    (void)modes;
}

void Channel::invite(Client &user)
{
    this->_memberList[&user] = INVITED;
}

void Channel::join(Client &user)
{
    this->_memberList[&user] = CONNECTED;
}

void Channel::setMode(bool isRemoved, int mask)
{
    if (isRemoved)
        this->_modList[mask] = false;
    else
        this->_modList[mask] = true;
}