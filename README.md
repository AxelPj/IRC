# my_irc

Bienvenue dans **my_irc** ! Ce projet est un serveur IRC (Internet Relay Chat) écrit en C++98. L'objectif est de comprendre le fonctionnement d'un protocole réseau texte en le réimplémentant : gestion de plusieurs clients simultanés avec `poll()`, authentification, salons de discussion, messages privés et commandes d'opérateur.

## Sommaire

- [À propos](#à-propos)
- [Fonctionnalités](#fonctionnalités)
- [Installation](#installation)
- [Utilisation](#utilisation)
- [Structure du projet](#structure-du-projet)
- [Contributions](#contributions)

## À propos

IRC est un protocole de communication textuel en temps réel. Les utilisateurs se connectent à un serveur avec un client IRC, échangent des messages privés et rejoignent des salons (channels) pour discuter en groupe.

**my_irc** implémente la partie serveur : il écoute sur un port TCP (IPv4), accepte les connexions entrantes, multiplexe tous les clients avec un seul appel à `poll()` et répond selon les numériques définis par la RFC 1459. Le serveur s'annonce sous le nom `DyingStar.localhost`.

Le client de référence utilisé pendant le développement est **HexChat** (fourni dans le dépôt sous `HexChat.app`). D'autres clients peuvent fonctionner, mais seul HexChat a été testé. Une connexion brute avec `nc` est aussi possible.

## Fonctionnalités

### 1. Connexion et authentification

- `PASS <mot de passe>` : authentifie le client si le serveur a été lancé avec un mot de passe. Un serveur sans mot de passe accepte tous les clients.
- `NICK <pseudo>` : définit ou change le pseudo (30 caractères maximum, ne commence pas par un chiffre, lettres, chiffres et `[]\`_^{}|-` autorisés, doit être unique).
- `USER <username> <hostname> <servername> :<realname>` : enregistre l'utilisateur.
- Une fois `NICK` et `USER` reçus, le serveur envoie les messages de bienvenue (`001`, `002`, `003`).
- Tant qu'il n'est pas enregistré, le client ne peut utiliser que `PASS`, `NICK` et `USER` (sinon erreur `451`). Un mauvais mot de passe entraîne la déconnexion.

### 2. Salons (channels)

- `JOIN #salon [clé]` : rejoint un salon ou le crée (le créateur devient opérateur). Les noms commencent par `#` et font au plus 50 caractères.
- `JOIN #a,#b clé1,clé2` : rejoint plusieurs salons en une commande.
- `JOIN 0` : quitte tous les salons.
- `PART #salon [raison]` : quitte un ou plusieurs salons (séparés par des virgules).
- `TOPIC #salon [:sujet]` : affiche ou modifie le sujet du salon.
- Un salon vide est automatiquement supprimé.

### 3. Messages

- `PRIVMSG <cible> :<message>` : envoie un message à un utilisateur ou à un salon, avec plusieurs cibles possibles séparées par des virgules.
- `PING` / `PONG` : messages de maintien de connexion.
- `WHO <pseudo|#salon>` : affiche les informations d'un utilisateur ou des membres d'un salon.
- `QUIT [:raison]` : se déconnecte du serveur et prévient les membres des salons.

### 4. Commandes d'opérateur

- `KICK #salon <pseudo> [raison]` : expulse un utilisateur du salon.
- `INVITE <pseudo> #salon` : invite un utilisateur dans un salon.
- `MODE #salon <+/-modes> [paramètres]` : modifie les modes du salon :
  - `i` : salon sur invitation uniquement
  - `t` : seul un opérateur peut modifier le sujet
  - `k` : clé (mot de passe) du salon
  - `l` : nombre maximum de membres
  - `o` : donne ou retire le statut d'opérateur

### 5. Filtre de mots interdits

Les pseudos, noms d'utilisateur, messages, sujets et raisons de départ sont comparés à la liste `srcs/Server/forbiddenWord.txt`. Chaque mot trouvé dans un message est remplacé par `****`. Un pseudo ou un nom d'utilisateur qui contient un mot interdit est refusé.

> Le fichier est lu avec un chemin relatif : lancez le serveur depuis la racine du projet pour que le filtre soit actif.

## Installation

```sh
git clone https://github.com/AxelPj/IRC.git
cd IRC
make
```

La compilation produit l'exécutable `ircserv` (flags `-Wall -Wextra -Werror -std=c++98`, objets dans `objs/`).

Règles du Makefile :

```sh
make          # compile l'exécutable ircserv
make clean    # supprime le dossier objs/
make fclean   # supprime objs/ et l'exécutable
make re       # recompile tout depuis zéro
```

## Utilisation

Lancer le serveur :

```sh
./ircserv <port> [mot de passe]
```

- `<port>` : port d'écoute, entre 1024 et 65535 (6667 en général).
- `[mot de passe]` : facultatif. S'il est fourni, les clients doivent l'envoyer avec `PASS`.

```sh
./ircserv 6667 secret
```

**Avec netcat :**

```sh
nc -C 127.0.0.1 6667
```

```
PASS secret
NICK alice
USER alice 0 * :Alice
JOIN #general
TOPIC #general :Bienvenue sur le salon
PRIVMSG #general :Bonjour à tous !
MODE #general +k motdepasse
INVITE bob #general
QUIT :À plus
```

**Avec HexChat :**

1. Lancez `HexChat.app` et renseignez au moins le `Nick name` et le `User name`.
2. Cliquez sur _Add_, puis _Edit..._.
3. Remplacez l'adresse du serveur par `localhost/6667` (nom du serveur/port).
4. Si le serveur a un mot de passe, saisissez-le dans _Password_.
5. Cliquez sur _Close_, puis sur _Connect_.

## Structure du projet

- `Makefile` : `compilation de l'exécutable ircserv`
- `srcs/main.cpp` : `lecture des arguments (port, mot de passe) et lancement du serveur`
- `include/Server.hpp` : `déclaration de la classe Server (sockets, clients, salons, commandes)`
- `include/Client.hpp` : `déclaration de la classe Client (fd, pseudo, utilisateur, état d'authentification)`
- `include/Channel.hpp` : `déclaration de la classe Channel (membres, modes, sujet, clé, limite)`
- `include/IRCNumerics.hpp` : `macros des réponses numériques et des messages IRC`
- `srcs/Server/Server.cpp` : `création du socket, boucle poll(), acceptation et déconnexion des clients`
- `srcs/Server/ServerParser.cpp` : `découpage des lignes reçues et aiguillage vers les commandes`
- `srcs/Server/ServerParserCmd.cpp` : `vérification des paramètres de chaque commande`
- `srcs/Server/ServerCmd.cpp` : `exécution des commandes (PASS, NICK, USER, PART, MODE, TOPIC, WHO...)`
- `srcs/Server/ServerUtils.cpp` : `envoi des messages, gestion des modes et filtre de mots interdits`
- `srcs/Server/forbiddenWord.txt` : `liste des mots interdits`
- `srcs/Client/Client.cpp` : `implémentation de la classe Client`
- `srcs/Channel/Channel.cpp` : `implémentation de la classe Channel`
- `HexChat.app/` : `client IRC de référence (macOS)`

## Contributions

Les contributions sont les bienvenues ! Pour proposer une amélioration :

1. Forkez le dépôt.
2. Créez une branche : `git checkout -b ma-fonctionnalite`
3. Commitez vos changements : `git commit -m "Ajout de ma fonctionnalité"`
4. Poussez la branche : `git push origin ma-fonctionnalite`
5. Ouvrez une Pull Request.
