#ifndef IRCNUMERICS_HPP
#define IRCNUMERICS_HPP

#define RPL_TOPIC(server, client, channel, topic) (":" + server + " 332 " + client + " " + channel + " " + topic)
#define RPL_TOPICWHOTIME(server, client, channel, nick, setat) (":" + server + " 333 " + client + " " + channel + nick + )
RPL_NAMREPLY
RPL_ENDOFNAMES

#define ERR_CHANNELISFULL(server, client, channel) (":" + server + " 471 " + client + " " + channel + " :Cannot join channel (+l)")
#define ERR_INVITEONLYCHAN(server, client, channel) (":" + server + " 473 " + client + " " + channel + " :Cannot join channel (+i)")
#define ERR_BANNEDFROMCHAN(server, client, channel) (":" + server + " 474 " + client + " " + channel + " :Cannot join channel (+b)")
#define ERR_BADCHANNELKEY(server, client, channel) (":" + server + " 475 " + client + " " + channel + " :Cannot join channel (+k)")
#define ERR_CHANOPRIVSNEEDED(server, client, channel) (":" + server + " 482 " + client + " " + channel + " :You're not channel operator")

#endif
