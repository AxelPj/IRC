#include "Channel.hpp"

Channel::Channel(Server serv, Client cli, std::string name) : _server(serv), _name(name)
{
	this->_userLimit = -1;	
	this->_memberList.insert({cli, true});
	this->_modes = 0; //bitmask in order p/s/i/t/n/m
}

Channel::~Channel()
{
}

void Channel::join(client user, std::string password)
{
	if (_banlist.contains(user)
	//ERR_BANNEDFROMCHAN
	if (_modes & 8 && !invited)
	//ERR_INVITEONLYCHAN
	if (_userLimit >= 0 && _memberList.size() >= _userLimit)
	//ERR_CHANNELISFULL
	if (pass != _key)
	//ERR_BADCHANNELKEY
	
	//Success
	_memberList.insert({user, false});
	//JOIN MESSAGE
	//RPL_TOPIC
	//RPL_TOPICWHOTIME
	//RPL_NAMREPLY
	//RPL_ENDOFNAMES
}

//cannot be called on non-existent channel, needs to be addressed elsewhere
void Channel::part(client user)
{
	if (_memberList.contains(user))
	{
		_memberList.remove(user);
		//PART MESSAGE
	}
	else
		//ERR_NOTONCHANNEL
}

void Channel::topic(Client user)
{
	//if topic
	//RPL_TOPIC
	//RPL_TOPICWHOTIME
	//else
	//RPL_NOTOPIC
}

void Channel::topic(Client user, std::string newTopic)
{
	if (!_memberList.contains(user))
		//ERR_NOTONCHANNEL
	else if (_modes & 2 && !memberList[user]) //topic protected
		//ERR_CHANOPRIVSNEEDED
	else
	{
		_topic = newTopic;
		//TOPIC MESSAGE
	}
}

void Channel::names(Client user)
{
	if (_modes & 16 && !memberList.contains(user)) //secret channel
	{
		//RPL_ENDOFNAMES
		return;
	}
	//iterator over member list, omit invisible if user is not in channel
}

void Channel::kick(Client issuer, Client target)
{
	if (memberList.count(issuer) == 0)
		//ERR_NOTONCHANNEL
	else if (!memberList[issuer])
		//ERR_CHANOPRIVSNEEDED
	else if (memberList.count(target) == 0)
		//ERR_USERNOTINCHANNEL
	else
	{
		memberList.erase(target);
		//KICK MESSAGE
	}
}

void Channel::mode(Client user, std::string modes)
{
	int		i = 0;
	bool	isRemoved = true;

	if (memberList.count(user) == 0)
		//ERR_NOTONCHANNEL
	if (!memberList[user])
		//ERR_CHANOPRIVSNEEDED
	//bitmask in order p/s/i/t/n/m
	while (modes[i])
	{
		if (modes[i] == '+' || modes[i] == '-')
			isRemoved = (modes[i++] == '-');
		else if (i == 0)
			//wrong usage
		else
			setMode(isRemoved, modes[i])
	}
}
