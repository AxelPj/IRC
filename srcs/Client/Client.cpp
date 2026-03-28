#include "Client.hpp"
#include <arpa/inet.h>

Client::Client() : _fd(-1) {}

Client::Client(sockaddr_in addrClient, pollfd Socketclient)
	: _fd(Socketclient.fd) {
	this->_adress = inet_ntoa(addrClient.sin_addr);
	std::cout << "New client connected: " << this->_adress << std::endl;
	std::cout << "Client fd: " << this->_fd << std::endl;
	std::cout << "Client port: " << ntohs(addrClient.sin_port) << std::endl;
	this->_buffer = "";
	this->_user = "";
}

Client &Client::operator=(const Client &other) {
	if (this != &other) 
	{
		this->_fd = other._fd;
		this->_password = other._password;
		this->_adress = other._adress;
		this->_nick = other._nick;
		this->_second = other._second;
		this->_third = other._third;
		this->_username = other._username;
	}
	return *this;
}

Client::~Client() {
}

std::string Client::getPassword() const {
	return this->_password;
}

std::string Client::getAdress() const {
	return this->_adress;
}

std::string Client::getNick() const {
	return this->_nick;
}

std::string Client::getSecond() const {
	return this->_second;
}

std::string Client::getThird() const {
	return this->_third;
}

std::string Client::getUser() const {
	return this->_username;
}

std::string	Client::getBuffer() const
{
	return(this->_buffer);
}

void Client::setAddBuffer(char *msg)
{
	this->_buffer += msg;
}

void Client::setRemoveBuffer() 
{
	this->_buffer.clear();
}

void Client::setUser(const std::string& user) 
{
	this->_user = user;
}

void Client::setNick(const std::string &newNick)
{
	if (newNick.size() <= 30)
		this->_nick = newNick;
	else
		this->_nick = (newNick.substr(0, 30));
}

int	Client::getFd() const
{
	return(this->_fd);
}

std::string Client::getUser() const
{
	return(this->_user);
}