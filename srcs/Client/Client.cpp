#include "Client.hpp"
#include <arpa/inet.h>

Client::Client() : _fd(-1) {}

Client::Client(sockaddr_in addrClient, pollfd Socketclient)
	: _fd(Socketclient.fd) {
	this->_address = inet_ntoa(addrClient.sin_addr);
	std::cout << "New client connected: " << this->_address << std::endl;
	std::cout << "Client fd: " << this->_fd << std::endl;
	std::cout << "Client port: " << ntohs(addrClient.sin_port) << std::endl;
	this->_buffer = "";
	this->_username = "";
	this->_realName = "";
	this->_nick = "";
	this->_isAway = false;
	this->_registered = false;
}

Client &Client::operator=(const Client &other) {
	if (this != &other) 
	{
		this->_fd = other._fd;
		this->_password = other._password;
		this->_address = other._address;
		this->_nick = other._nick;
		this->_realName = other._realName;
		this->_username = other._username;
		this->_registered = other._registered;
	}
	return *this;
}

Client::~Client() {
}

std::string Client::getPassword() const {
	return this->_password;
}

std::string Client::getAddress() const {
	return this->_address;
}

std::string Client::getNick() const {
	return this->_nick;
}

std::string Client::getUser() const {
	return this->_username;
}

std::string	Client::getBuffer() const
{
	return(this->_buffer);
}

std::string Client::getRealName() const
{
	return(this->_realName);
}

int	Client::getFd() const
{
	return(this->_fd);
}

bool Client::isAway() const
{
	return (this->_isAway);
}

bool	Client::getRegistered() const
{
	return (this->_registered);
}

void Client::setAddBuffer(char *msg)
{
	this->_buffer += msg;
}

void Client::setRemoveBuffer() 
{
	this->_buffer.clear();
}

void Client::setBuffer(const std::string &buffer)
{
	this->_buffer = buffer;
}

void Client::setNick(const std::string &newNick)
{
	if (newNick.size() <= 30)
		this->_nick = newNick;
	else
		this->_nick = (newNick.substr(0, 30));
}

void	Client::setUser(const std::string& user)
{
	this->_username = user;
}

void	Client::setRealName(const std::string& name)
{
	this->_realName = name;
}

void	Client::setRegistered(bool registered)
{
	this->_registered = registered;
}

char	Client::getAwayStatus()
{
	if (_isAway)
		return 'G';
	else
		return 'H';
}

//This function implemented in prevision for the addition of secret channels and user modes
bool	Client::canSee(const Client &client)
{
	(void) client;
	return true;
}

//This function implemented in prevision for the addition of secret channels and user modes
bool	Client::canSee(const Channel& channel)
{
	(void) channel;
	return true;
}
