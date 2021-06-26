/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/19 11:03:45 by mondrew           #+#    #+#             */
/*   Updated: 2021/06/20 18:07:46 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

typedef struct		s_client {
	int				id;
	int				socket;
	char			*msgToClient;
	char			*msgFromClient;
	int				deleteMe;
	struct s_client	*next;
}					t_client;

static const char	*nameMsg = "";
static const char	*arrivedMsg = "";
static const char	*leftMsg = "";
static int			number_of_clients = 0;

int		runMainLoop(int serverSocket)
{
	fd_set		rds;
	fd_set		wrs;
	int			maxFd;
	t_client	*clientNode = NULL;

	while (1)
	{
		// Clean
		FD_ZERO(&rds);
		FD_ZERO(&wrs);
		maxFd = findMaxFd(serverSocket, clientNode);

		// Fill
		FD_SET(serverSocket, &rds);
		t_client	*tmp1 = clientNode;
		while (tmp1)
		{
			FD_SET(tmp1->socket, &rds);
			FD_SET(tmp1->socket, &wrs);
			tmp1 = tmp1->next;
		}

		// select
		int		ret = select(maxFd + 1, &rds, &wrs, 0, 0);
		if (ret < 0)
		{
			if (errno == EINTR)
				continue ;
			else
			{
				freeClients(clientNode);
				return (-1);
			}
		}
		else if (ret == 0)
			continue ;
		else
		{
			if (FD_ISSET(serverSocket, &rds))
			{
				if (acceptClient(serverSocket, &clientNode) == -1)
				{
					freeClients(clientNode);
					return (-1);
				}
			}
			t_client	*tmp = clientNode;
			while (tmp)
			{
				if (tmp->msgToClient && strlen(tmp->msgToClient) && FD_ISSET(tmp->socket, &wrs))
				{
					if (sendMsgToClient(tmp) == -1)
					{
						freeClients(clientNode);
						return (-1);
					}
				}
				else if (FD_ISSET(tmp->socket, &rds))
				{
					if (recvMsgFromClient(tmp, &clientNode) == -1)
					{
						freeClients(clientNode);
						return (-1);
					}
					if (resendMessages(tmp, &clientNode) == -1)
					{
						freeClients(clientNode);
						return (-1);
					}
				}
				tmp = tmp->next;
			}
		}
		deleteDisconnectedClients(&clientNode);
	}
	return (0);
}

int		startServer(int port)
{
	int					serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in	serverAddr;

	if (serverSocket == -1)
		return (-1);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
	serverAddr.sin_port = htons(port);

	if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
	{
		close(serverSocket);
		return (-1);
	}

	if (listen(serverSocket, 128) == -1)
	{
		close(serverSocket);
		return (-1);
	}
	return (serverSocket);
}

int		main(int argc, char **argv)
{
	int		serverSocket;
	int		port;

	if (argc != 2)
	{
		printError("Wrong number of arguments\n");
		return (1);
	}
	port = atoi(argv[1]);
	if ((serverSocket = startServer(port)) == -1)
	{
		printError("Fatal error\n");
		return (1);
	}
	if (runMainLoop(serverSocket) == -1)
	{
		printError("Fatal error\n");
		close(serverSocket);
		return (1);
	}
	return (0);
}
