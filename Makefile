# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: gdelhota <gdelhota@student.42perpignan.fr  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/03/06 15:31:02 by gdelhota          #+#    #+#              #
#    Updated: 2026/03/18 14:39:55 by gdelhota         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

CXX = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98

SRC_DIR = srcs/
OBJ_DIR = objs/
INCLUDES = include/

SRCS = srcs/main.cpp srcs/Server/Server.cpp srcs/Client/Client.cpp srcs/Channel/Channel.cpp srcs/Server/ServerCmd.cpp srcs/Server/ServerParser.cpp srcs/Server/ServerUtils.cpp
OBJS = $(SRCS:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)

# RULES

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) -I$(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $(CPPFLAGS) -I$(INCLUDES) $(OBJS) -o $(NAME)

all: $(NAME)

clean:
	rm -rf $(OBJ_DIR)
	
fclean: clean
	rm -f $(NAME)

re: fclean all
