
#ifndef IRCNUMERICS_H
#define IRCNUMERICS_H

#define RPL_NOTOPIC(server, client, channel) (":" + server + " 331 " + channel + " :No topic is set\r\n")
#define RPL_TOPIC(server, client, channel, topic) (":" + server + " 332 " + client + " " + channel + " " + topic + "\r\n")
#define RPL_INVITING(server, client, nick, channel) (":" + server + " 341 " + client + " " + nick + " " + channel + "\r\n")
#define RPL_NAMREPLY(server, channel, users) (":" + server + " 353 " + channel + " " + users + "\r\n")
#define RPL_ENDOFNAMES(server, client, channel) (":" + server + " 366 " + channel + " :End of /NAMES list\r\n")

#define ERR_NOSUCHNICK(server, nick) (":" + server + " 401 " + nick + " :No such nick/channel\r\n")
#define ERR_NOSUCHCHANNEL(server, channel) (":" + server + " 403 " + channel + " :No such channel\r\n")
#define ERR_UNKNOWNCOMMAND(server, command) (":" + server + " 421 " + command + " :Unknown command\r\n")
#define ERR_NONICKNAMEGIVEN(server, client) (":" + server + " 431 " + client + " :No nickname given\r\n")
#define ERR_NICKNAMEINUSE(server, client, nick) (":" + server + " 433 " + client + " " + nick " + :Nickname is already in use\r\n")
#define ERR_USERNOTINCHANNEL(server, nick, channel) (":" + server + " 441 " + nick + " " + channel + " :They aren't on that channel\r\n")
#define ERR_NOTONCHANNEL(server, client, channel) (":" + server + " 442 " + client + " " + channel + " :You're not on that channel\r\n")
#define ERR_NEEDMOREPARAMS(server, client, command) (":" + server + " 461 " + command + " :Not enough parameters\r\n")
#define ERR_CHANNELISFULL(server, client, channel) (":" + server + " 471 " + client + " " + channel + " :Cannot join channel (+l)\r\n")
#define ERR_UNKNOWNMODE(server, client, mode) (":" + server + " 472 " + client + " " + mode + " :is unknown mode char to me\r\n")
#define ERR_INVITEONLYCHAN(server, client, channel) (":" + server + " 473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")
#define ERR_BANNEDFROMCHAN(server, client, channel) (":" + server + " 474 " + client + " " + channel + " :Cannot join channel (+b)\r\n")
#define ERR_BADCHANNELKEY(server, client, channel) (":" + server + " 475 " + client + " " + channel + " :Cannot join channel (+k)\r\n")
#define ERR_CHANOPRIVSNEEDED(server, client, channel) (":" + server + " 482 " + client + " " + channel + " :You're not channel operator\r\n")

#endif
