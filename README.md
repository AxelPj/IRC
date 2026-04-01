_This project has been created as part of the 42 curriculum by axelpeti, gdelhota, lesalmon_

# ft_irc

## Description
The goal of this project is to recreate an IRC server in C++.
IRC (Internet Relay Chat) is a text-based communication protocol on the Internet.
It offers real-time messaging that can be either public or private. Users can exchange direct messages and join group channels using an IRC client that connects to the server.

## Instructions
Build the project using _Make_ and launch the server with
```./ircserv <port> <password>```
where `<port>` is the port your server will be listening onto (typically 6667) and `<password>` (optionnal) is the password that clients will have to enter in order to connect to the server.

Once the server is up and running, you can connect to it using an IRC client or `nc` command.
Hexchat was the client we used as reference during developpement and testing and thus the only one that is officially supported. Other clients may be able to connect and send/receive message but we cannot guarantee that you encounter no issues with them.

To connect to the server, you have to launch _Hexchat.app_, enter your informations (at least `Nick name` and `User name`), click on _Add_ then _Edit..._.
Now you have to modify the server name to `localhost/6667` (name of the server/port of the server). If the password is set, enter it in _Password_.
Once that is done, click _close_, go back to the main menu and _connect_ your Hexchat client to the server.

To connect to the server through the terminal, launch the server, then execute the following command:
```nc -C 127.0.0.1 6667```

You will then be able to send messages to the client by simply pressing enter

## Resources

### IRC Protocol Documentation:
- https://datatracker.ietf.org/doc/html/rfc1459 
- https://modern.ircdocs.horse

### Hexchat Documentation:
- https://hexchat.readthedocs.io/en/latest/
### Why did we choose Hexchat as our reference client ?
- We love open source: As developpers, we firmly believe that it is important to keep as much of our tools and product open-source. This is why we chose Hexchat as reference when developping this project.

### Networking ressources for communication over IP
- https://bien-programmer.fr/reseaux.php

### Project Organization
- https://miro.com/app/
- We used a Miro board to keep things organized during the whole developpement process, centralizing brainstormings, external resources, kanbans and UML diagrams in one place that was accessible to everyone

### AI usage
- We occasionally used AI assistance to speed up the completion of repetitive code patterns, especially when the structure was already defined and only mechanical implementation work remained.
- AI was also used as a support tool to clarify the meaning of some compiler errors and a few IRC protocol messages encountered during development and testing.
