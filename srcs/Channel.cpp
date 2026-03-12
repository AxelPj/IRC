#include "Channel.hpp"

Channel::Channel(Server serv, Client cli, std::string name) : _server(serv), _name(name)
{
	this->_modes = 0;
	this->_userLimit = -1;	
	this->_memberList.insert({cli, true});

}

Channel::~Channel()
{
}
