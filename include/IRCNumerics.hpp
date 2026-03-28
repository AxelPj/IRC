#ifndef IRCNUMERICS_HPP
#define IRCNUMERICS_HPP

#include <string>

#define RPL_NOTOPIC(server, client, channel) (std::string(":") + server + " 331 " + client + " " + channel + " :No topic is set\r\n")
#define RPL_TOPIC(server, client, channel, topic) (std::string(":") + server + " 332 " + client + " " + channel + " " + topic + "\r\n")
#define RPL_INVITING(server, client, nick, channel) (std::string(":") + server + " 341 " + client + " " + nick + " " + channel + "\r\n")
#define RPL_NAMREPLY(server, client, channel, users) (std::string(":") + server + " 353 " + client + " = " + channel + " :" + users + "\r\n")
#define RPL_ENDOFNAMES(server, client, channel) (std::string(":") + server + " 366 " + client + " " + channel + " :End of /NAMES list\r\n")
#define RPL_CHANNELMODEIS(server, client, channel, modes) (std::string(":") + server + " 324 " + client + " " + channel + " " + modes + "\r\n")
#define RPL_CREATIONTIME(server, client, channel, timestamp) (std::string(":") + server + " 329 " + client + " " + channel + " " + timestamp + "\r\n")

#define ERR_NOSUCHNICK(server, nick) (std::string(":") + server + " 401 " + nick + " :No such nick/channel\r\n")
#define ERR_NOSUCHCHANNEL(server, channel) (std::string(":") + server + " 403 " + channel + " :No such channel\r\n")
#define ERR_UNKNOWNCOMMAND(server, command) (std::string(":") + server + " 421 " + command + " :Unknown command\r\n")
#define ERR_NONICKNAMEGIVEN(server, client) (std::string(":") + server + " 431 " + client + " :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME(server, nick) (std::string(":") + server + " 432 " + nick + " :Erroneous nickname\r\n")
#define ERR_NICKNAMEINUSE(server, client, nick) (std::string(":") + server + " 433 " + client + " " + nick + " :Nickname is already in use\r\n")
#define ERR_USERNOTINCHANNEL(server, nick, channel) (std::string(":") + server + " 441 " + nick + " " + channel + " :They aren't on that channel\r\n")
#define ERR_NOTONCHANNEL(server, client, channel) (std::string(":") + server + " 442 " + client + " " + channel + " :You're not on that channel\r\n")
#define ERR_NEEDMOREPARAMS(server, client, command) (std::string(":") + server + " 461 " + command + " :Not enough parameters\r\n")
#define ERR_NORECIPIENT(server, client) (std::string(":") + server + " 411 " + client + " :No recipient given (PRIVMSG)\r\n")
#define ERR_NOTEXTOSEND(server, client) (std::string(":") + server + " 412 " + client + " :No text to send\r\n")
#define ERR_CHANNELISFULL(server, client, channel) (std::string(":") + server + " 471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_UNKNOWNMODE(server, client, mode) (std::string(":") + server + " 472 " + client + " " + mode + " :is unknown mode char to me\r\n")
#define ERR_INVITEONLYCHAN(server, client, channel) (std::string(":") + server + " 473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BANNEDFROMCHAN(server, client, channel) (std::string(":") + server + " 474 " + client + " " + channel + " :Cannot join channel (+b)\r\n")
#define ERR_BADCHANNELKEY(server, client, channel) (std::string(":") + server + " 475 " + client + " " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_CHANOPRIVSNEEDED(server, client, channel) (std::string(":") + server + " 482 " + client + " " + channel + " :You're not channel operator\r\n")
#define ERR_USERONCHANNEL(server, client, nick, channel) (std::string(":") + server + " 443 " + client + " " + nick + " " + channel + " :is already on channel\r\n")
#define ERR_INVALIDMODEPARAM() (std::string(":server 696 :Invalid mode parameter\r\n"))

#endif
