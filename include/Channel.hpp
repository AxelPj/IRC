#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <map>

class Server;
class Client;

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
    NOT_CONNECTED = 0,
    CONNECTED     = 1,
    INVITED       = 2,
    OP            = 3
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
        void    						        invite(Client &user);

    public:
        								        Channel();
        								        Channel(const Client &cli, const std::string &name);
        								        ~Channel();

        // Cmds
        void    						        kick(Client &issuer, Client &target);
        void    						        mode(Client &user, const std::string &modes);
        void    						        names(Client &user);
        void    						        topic(Client &user, const std::string &newTopic);
        void    						        topic(Client &user);
        void    						        part(Client &user);
        void    						        join(Client &user, const std::string &password);

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
        bool    						        isInvited(Client &client) const;
        bool    						        isMember(Client &client) const;
        bool    						        isOp(Client &client) const;
		bool    						        *whichMod();
};

#endif