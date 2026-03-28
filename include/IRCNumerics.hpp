#ifndef IRCNUMERICS_HPP
#define IRCNUMERICS_HPP

#include <string>

#define RPL_WELCOME(server, nick, user, host) (std::string(":") + server + " 001 " + nick + " :Welcome to the IRC Network " + nick + "!" + user + "@" + host + "\r\n")
#define RPL_YOURHOST(server, nick, version) (std::string(":") + server + " 002 " + nick + " :Your host is " + server + ", running version " + version + "\r\n")
#define RPL_CREATED(server, nick, createdAt) (std::string(":") + server + " 003 " + nick + " :This server was created " + createdAt + "\r\n")
#define RPL_MYINFO(server, nick, version, userModes, chanModes) (std::string(":") + server + " 004 " + nick + " " + server + " " + version + " " + userModes + " " + chanModes + "\r\n")
#define RPL_MOTDSTART(server, nick) (std::string(":") + server + " 375 " + nick + " :- " + server + " Message of the Day -\r\n")
#define RPL_MOTD(server, nick, text) (std::string(":") + server + " 372 " + nick + " :- " + text + "\r\n")
#define RPL_ENDOFMOTD(server, nick) (std::string(":") + server + " 376 " + nick + " :End of /MOTD command.\r\n")
#define ERR_NOMOTD(server, nick) (std::string(":") + server + " 422 " + nick + " :MOTD File is missing\r\n")

#define MSG_JOIN(nick, user, host, channel) (std::string(":") + nick + "!" + user + "@" + host + " JOIN " + channel + "\r\n")
#define MSG_PART(nick, user, host, channel, reason) (std::string(":") + nick + "!" + user + "@" + host + " PART " + channel + " :" + reason + "\r\n")
#define MSG_QUIT(nick, user, host, reason) (std::string(":") + nick + "!" + user + "@" + host + " QUIT :" + reason + "\r\n")
#define MSG_NICK(oldNick, user, host, newNick) (std::string(":") + oldNick + "!" + user + "@" + host + " NICK :" + newNick + "\r\n")

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
#define ERR_NOTREGISTERED(server, client) (std::string(":") + server + " 451 " + client + " :You have not registered\r\n")
#define ERR_NEEDMOREPARAMS(server, client, command) (std::string(":") + server + " 461 " + client + " " + command + " :Not enough parameters\r\n")
#define ERR_NORECIPIENT(server, client) (std::string(":") + server + " 411 " + client + " :No recipient given (PRIVMSG)\r\n")
#define ERR_NOTEXTOSEND(server, client) (std::string(":") + server + " 412 " + client + " :No text to send\r\n")
#define ERR_CANNOTSENDTOCHAN(server, client, channel) (std::string(":") + server + " 404 " + client + " " + channel + " :Cannot send to channel\r\n")
#define ERR_CHANNELISFULL(server, client, channel) (std::string(":") + server + " 471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_UNKNOWNMODE(server, client, mode) (std::string(":") + server + " 472 " + client + " " + mode + " :is unknown mode char to me\r\n")
#define ERR_INVITEONLYCHAN(server, client, channel) (std::string(":") + server + " 473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BANNEDFROMCHAN(server, client, channel) (std::string(":") + server + " 474 " + client + " " + channel + " :Cannot join channel (+b)\r\n")
#define ERR_BADCHANNELKEY(server, client, channel) (std::string(":") + server + " 475 " + client + " " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_CHANOPRIVSNEEDED(server, client, channel) (std::string(":") + server + " 482 " + client + " " + channel + " :You're not channel operator\r\n")
#define ERR_USERONCHANNEL(server, client, nick, channel) (std::string(":") + server + " 443 " + client + " " + nick + " " + channel + " :is already on channel\r\n")
#define ERR_INVALIDMODEPARAM() (std::string(":server 696 :Invalid mode parameter\r\n"))
#define ERR_ALREADYREGISTERED(server, client) (std::string(":") + server + " 462 " + client + " :You may not reregister\r\n")
#define ERR_INVALIDUSERNAME(server, client) (std::string(":") + server + " 468 " + client + " :Erroneous username\r\n")


//A VERIFIER SI ON A BESOIN DE CES NUMERICS

// Additional RPL responses
#define RPL_AWAY(server, nick, message) (std::string(":") + server + " 306 " + nick + " :" + message + "\r\n")
#define RPL_UNAWAY(server, nick) (std::string(":") + server + " 305 " + nick + " :You are no longer marked as being away\r\n")
#define RPL_WHOREPLY(server, client, channel, user, host, nick, flags, hopcount, realname) (std::string(":") + server + " 352 " + client + " " + channel + " " + user + " " + host + " " + nick + " " + flags + " :" + hopcount + " " + realname + "\r\n")
#define RPL_ENDOFWHO(server, client, channel) (std::string(":") + server + " 315 " + client + " " + channel + " :End of /WHO list\r\n")
#define RPL_LIST(server, client, channel, usercount, topic) (std::string(":") + server + " 322 " + client + " " + channel + " " + usercount + " :" + topic + "\r\n")
#define RPL_LISTEND(server, client) (std::string(":") + server + " 323 " + client + " :End of /LIST\r\n")
#define RPL_TOPICWHOTIME(server, client, channel, who, timestamp) (std::string(":") + server + " 333 " + client + " " + channel + " " + who + " " + timestamp + "\r\n")
#define RPL_UMODEIS(server, nick, modes) (std::string(":") + server + " 221 " + nick + " +" + modes + "\r\n")
#define RPL_KICKOUT(nick, user, host, channel, reason) (std::string(":") + nick + "!" + user + "@" + host + " KICK " + channel + " :" + reason + "\r\n")
#define RPL_KILL(nick, user, host, reason) (std::string(":") + nick + "!" + user + "@" + host + " KILL :" + reason + "\r\n")

// Additional ERR responses  
#define ERR_NOSUCHSERVER(server, servername) (std::string(":") + server + " 402 " + servername + " :No such server\r\n")
#define ERR_TOOMANYCHANNELS(server, client, channel) (std::string(":") + server + " 405 " + client + " " + channel + " :You have joined too many channels\r\n")
#define ERR_TOOMANYTARGETS(server, client, target) (std::string(":") + server + " 407 " + client + " " + target + " :Too many recipients\r\n")
#define ERR_NOCOMMANDGIVEN(server, client) (std::string(":") + server + " 409 " + client + " :No origin specified\r\n")
#define ERR_NOADMININFO(server, servername) (std::string(":") + server + " 423 " + servername + " :No administrative info available\r\n")
#define ERR_FILEERROR(server, fileop) (std::string(":") + server + " 424 :File error doing " + fileop + "\r\n")
#define ERR_USERSDONTMATCH(server, client) (std::string(":") + server + " 502 " + client + " :Cant change mode for other users\r\n")
#define ERR_RESTRICTED(server, client) (std::string(":") + server + " 499 " + client + " :Your connection is restricted!\r\n")
#define ERR_KEYSET(server, client, channel) (std::string(":") + server + " 467 " + client + " " + channel + " :Channel key already set\r\n")

// Additional messages
#define MSG_KICK(nick, user, host, channel, target, reason) (std::string(":") + nick + "!" + user + "@" + host + " KICK " + channel + " " + target + " :" + reason + "\r\n")
#define MSG_TOPIC(nick, user, host, channel, topic) (std::string(":") + nick + "!" + user + "@" + host + " TOPIC " + channel + " :" + topic + "\r\n")
#define MSG_MODE(nick, user, host, channel, modes, params) (std::string(":") + nick + "!" + user + "@" + host + " MODE " + channel + " " + modes + " " + params + "\r\n")
#define MSG_PRIVMSG(nick, user, host, target, message) (std::string(":") + nick + "!" + user + "@" + host + " PRIVMSG " + target + " :" + message + "\r\n")
#define MSG_NOTICE(nick, user, host, target, message) (std::string(":") + nick + "!" + user + "@" + host + " NOTICE " + target + " :" + message + "\r\n")

#endif
