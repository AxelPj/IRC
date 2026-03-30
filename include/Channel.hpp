#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>

class Server;
class Client;

#define RPL_NAMREPLY(server, client, channel, users) (std::string(":") + server + " 353 " + client + " = " + channel + " :" + users + "\r\n")

enum ChannelMode
{
	INVITE_ONLY = 0, // i
	PASSWORD    = 1, // k
	LIMIT       = 2, // l
	TOPIC_OPE   = 3, // t
	LIST_MODE   = 4
};

enum ClientStatus
{
	CONNECTED     = 0,
	INVITED       = 1,
	OP            = 2
};

class Channel
{
	private:
		std::string                             _name;
		std::string                             _password;
		std::string                             _topic;
		int                                     _userLimit;
		bool                                    _modList[LIST_MODE];
		std::map<const Client*, ClientStatus>   _memberList;
		void    						        join(Client &user);
		void    						        setMode(bool isRemoved, int mask);

	public:
												Channel();
												Channel(const Client &cli, const std::string &name);
												~Channel();

		// Cmds
		void							        kick(Client &issuer, Client &target);
		void							        mode(Client &user, const std::string &modes);
		void							        part(Client &user);
		std::string						        namesReply(const Client &user);

		// getters
		const std::string                       &getName() const;
		const std::string                       &getTopic() const;
		const std::string                       &getPassword() const;
		int                                     getUserLimit() const;
		bool                                    *getModList();
		int                                     getStatusClient(const Client &client) const;
		std::map<const Client*, ClientStatus>   &getMemberList();

		// setters
		void    						        setName(const std::string &name);
		void    						        setTopic(const std::string &topic);
		void    						        setPassword(const std::string &password);
		void    						        setUserLimit(int limit);
		void    						        setStatusClient(const Client &client, ClientStatus status);
		bool                                    setInviteOnly(bool active);
		bool                                    setTopicResctriction(bool active);

		// utils
		bool    						        isInvited(const Client &client) const;
		bool    						        isMember(const Client &client) const;
		bool    						        isOp(const Client &client) const;
		void    						        invite(Client &user);
		bool    						        *whichMod();
		void                        			removeMember(const Client &client);

};

#endif
