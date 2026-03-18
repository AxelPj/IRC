#include "Client.hpp"
#include <arpa/inet.h>

Client::Client() : _ops(false), _fd(-1) {
}

Client::Client(sockaddr_in addrClient, pollfd Socketclient)
	: _ops(false), _fd(Socketclient.fd) {
	this->_adress = inet_ntoa(addrClient.sin_addr);
	std::cout << "New client connected: " << this->_adress << std::endl;
	std::cout << "Client fd: " << this->_fd << std::endl;
	std::cout << "Client port: " << ntohs(addrClient.sin_port) << std::endl;
}

Client &Client::operator=(const Client &other) {
	if (this != &other) {
		this->_ops = other._ops;
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

std::string Client::getPassword() {
	return this->_password;
}

std::string Client::getAdress() {
	return this->_adress;
}

std::string Client::getNick() {
	return this->_nick;
}

std::string Client::getSecond() {
	return this->_second;
}

std::string Client::getThird() {
	return this->_third;
}

std::string Client::getUser() {
	return this->_username;
}

std::string	Client::getBuffer()
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

int Client::cmdNick(const std::string& newNick, const Client& user) 
{
	(void)user;
	for (size_t i = 0; i < newNick.size(); ++i) {
		if (newNick[i] == ' ' || newNick[i] == ',' || newNick[i] == '*' ||
			newNick[i] == '?' || newNick[i] == '!' || newNick[i] == '@') {
			return -1; // Invalid character in nickname
		}
	}
	this->_nick = newNick.substr(0, 30);
	return 0;
}

int	Client::getfd()
{
	return(this->_fd);
}

int Client::cmdQuit(const Client& user) {
	(void)user;
	return 0;
}

int Client::cmdReconnect(const Client& user) {
	(void)user;
	return 0;
}
