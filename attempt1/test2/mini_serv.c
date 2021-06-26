/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/18 21:10:52 by mondrew           #+#    #+#             */
/*   Updated: 2021/06/20 18:14:33 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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

static const char	*nameMsg = "client %d: ";
static const char	*arrivedMsg = "server: client %d just arrived\n";
static const char	*leftMsg = "server: client %d just left\n";
static int			number_of_clients = 0;

char	*ft_getMsgWithId(const char *msg, int id)
{
	char	*newMsg = malloc(sizeof(char) * (strlen(msg) + 10 + 1));
	if (!newMsg)
		return (NULL);
	sprintf(newMsg, msg, id);
	return (newMsg);
}

// #15
char	*ft_substr(char *str, int start, int length)
{
	int		i = 0;
	int		j = start;

	char	*ret = malloc(sizeof(char) * (length + 1));
	if (!ret)
		return (NULL);

	while (str && str[j] && length > 0)
	{
		ret[i] = str[j];
		i++;
		j++;
		length--;
	}
	ret[i] = '\0';
	return (ret);
}

// #14
char	*ft_strjoin(char *str, char *toAdd)
{
	int		i = 0;
	int		j = 0;

	if (!str)
		return (ft_strjoin("", toAdd));
	char	*ret = malloc(sizeof(char) * (strlen(str) + strlen(toAdd) + 1));
	if (!ret)
		return (NULL);
	while (str[i])
	{
		ret[i] = str[i];
		i++;
	}
	while (toAdd[j])
	{
		ret[i] = toAdd[j];
		i++;
		j++;
	}
	ret[i] = '\0';
	return (ret);
}

// #13
int		findNewLine(char *str)
{
	int		i = 0;

	if (!str)
		return (-1);
	while (str[i])
	{
		if (str[i] == '\n')
			return (i);
		i++;
	}
	return (-1);
}

// #12
int		findMaxFd(int serverSocket, t_client *clientNode)
{
	int		maxFd = serverSocket;

	while (clientNode)
	{
		if (clientNode->socket > maxFd)
			maxFd = clientNode->socket;
		clientNode = clientNode->next;
	}
	return (maxFd);
}

// #11
void	printError(char *msg)
{
	write(2, msg, strlen(msg));
}

// #10
void	deleteClient(t_client *del, t_client **clientNode)
{
	t_client	*tmp = *clientNode;

	if (*clientNode == del)
	{
		// First elem
		*clientNode = (*clientNode)->next;

		close(del->socket);
		if (del->msgToClient)
			free(del->msgToClient);
		if (del->msgFromClient)
			free(del->msgFromClient);
		free(del);
	}
	else
	{
		while (tmp && tmp->next != del)
			tmp = tmp->next;
		if (!tmp)
			return ;
		tmp->next = del->next;

		close(del->socket);
		if (del->msgToClient)
			free(del->msgToClient);
		if (del->msgFromClient)
			free(del->msgFromClient);
		free(del);
	}
}

void	deleteDisconnectedClients(t_client **clientNode)
{
	t_client	*tmp = *clientNode;

	while (tmp)
	{
		if (tmp->deleteMe == 1)
		{
			t_client	*del = tmp;
			tmp = tmp->next;
			deleteClient(del, clientNode);
		}
		else
			tmp = tmp->next;
	}
}

// #9
void	freeClients(t_client *clientNode)
{
	t_client	*tmp = clientNode;

	while (clientNode)
	{
		tmp = clientNode;
		clientNode = clientNode->next;

		close(tmp->socket);
		if (tmp->msgToClient)
			free(tmp->msgToClient);
		if (tmp->msgFromClient)
			free(tmp->msgFromClient);
		free(tmp);
	}
}

// #8
int		sendAll(char *msg, t_client **clientNode, t_client *except)
{
	t_client	*tmp = *clientNode;

	while (tmp)
	{
		if (tmp != except)
		{
			char	*newStr = ft_strjoin(tmp->msgToClient, msg);
			if (!newStr)
				return (-1);
			free(tmp->msgToClient);
			tmp->msgToClient = newStr;
		}
		tmp = tmp->next;
	}
	return (0);
}

// #7 
int		resendMessages(t_client *client, t_client **clientNode)
{
	int			i = 0;

	while ((i = findNewLine(client->msgFromClient)) != -1)
	{
		char	*newNameMsg = ft_getMsgWithId(nameMsg, client->id);
		if (!newNameMsg)
			return (-1);
		char	*rawMsg = ft_substr(client->msgFromClient, 0, i + 1);
		if (!rawMsg)
		{
			free(newNameMsg);
			return (-1);
		}
		char	*fullMsg = ft_strjoin(newNameMsg, rawMsg);
		if (!fullMsg)
		{
			free(newNameMsg);
			free(rawMsg);
			return (-1);
		}
		free(newNameMsg);
		free(rawMsg);

		if (sendAll(fullMsg, clientNode, NULL) == -1)
		{
			free(fullMsg);
			return (-1);
		}
		free(fullMsg);

		// Trancate original string
		char	*newStr = ft_substr(client->msgFromClient, i + 1, strlen(client->msgFromClient) - (i + 1));
		if (!newStr)
			return (-1);
		free(client->msgFromClient);
		client->msgFromClient = newStr;
	}
	return (0);
}

// #6
int		recvMsgFromClient(t_client *client, t_client **clientNode)
{
	char	buf[1024];

	memset(buf, 0, 1024);

	int		ret = read(client->socket, buf, 1023);
	if (ret < 0)
		return (-1);
	else if (ret == 0)
	{
		char	*newLeftMsg = ft_getMsgWithId(leftMsg, client->id);
		if (!newLeftMsg)
			return (-1);
		if (sendAll(newLeftMsg, clientNode, client) == -1)
		{
			free(newLeftMsg);
			return (-1);
		}
		free(newLeftMsg);
		client->deleteMe = 1;
	}
	else
	{
		buf[ret] = '\0';
		char	*newMsg = ft_strjoin(client->msgFromClient, buf);
		if (!newMsg)
			return (-1);
		free(client->msgFromClient);
		client->msgFromClient = newMsg;
	}
	return (0);
}

// #5
int		sendMsgToClient(t_client *client)
{
	int		i = findNewLine(client->msgToClient);

	if (i == -1)
		return (0);
	int		ret = write(client->socket, client->msgToClient, i + 1);
	if (ret == -1)
		return (-1);
	else
	{
		// Trancate original str
		char	*newMsg = ft_substr(client->msgToClient, ret, strlen(client->msgToClient) - ret);
		if (!newMsg)
			return (-1);
		free(client->msgToClient);
		client->msgToClient = newMsg;
	}
	return (0);
}

// #4
int		acceptClient(int serverSocket, t_client **clientNode)
{
	struct sockaddr_in	clientAddr;
	socklen_t			len = sizeof(clientAddr);

	// Accept client
	int					clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &len);

	if (clientSocket == -1)
		return (-1);

	// Create newClient
	t_client	*newClient = malloc(sizeof(t_client));
	if (!newClient)
	{
		close(clientSocket);
		return (-1);
	}
	// Fill newClient
	newClient->id = number_of_clients;
	number_of_clients++;
	newClient->socket = clientSocket;
	if (!(newClient->msgToClient = ft_strjoin("", "")))
	{
		close(serverSocket);
		free(newClient);
		return (-1);
	}
	if (!(newClient->msgFromClient = ft_strjoin("", "")))
	{
		close(serverSocket);
		free(newClient->msgToClient);
		free(newClient);
		return (-1);
	}
	newClient->deleteMe = 0;
	newClient->next = NULL;

	// Add newClient to client list
	if (!(*clientNode))
		*clientNode = newClient;
	else
	{
		t_client	*tmp = *clientNode;

		while (tmp->next)
			tmp = tmp->next;
		tmp->next = newClient;
	}

	// SendAll 'arrived' msg
	char	*newArrivedMsg = ft_getMsgWithId(arrivedMsg, newClient->id);
	if (!newArrivedMsg)
		return (-1);
	if (sendAll(newArrivedMsg, clientNode, newClient) == -1)
	{
		free(newArrivedMsg);
		return (-1);
	}
	free(newArrivedMsg);
	return (0);
}

// #3
int		runMainLoop(int serverSocket)
{
	fd_set		rds;
	fd_set		wrs;
	t_client	*clientNode = NULL;
	int			maxFd;

	while (1)
	{
		// Clear fd sets
		FD_ZERO(&rds);
		FD_ZERO(&wrs);
		maxFd = findMaxFd(serverSocket, clientNode);

		// Fill fd sets
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
		if (ret < 0) // new loop
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

// #2
int		startServer(int port)
{
	struct sockaddr_in	serverAddr;

	// Create a server socket
	int					serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (serverSocket == -1)
		return (-1);

	// Fill the struct
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
	serverAddr.sin_port = htons(port);

	// Bind server Socket to addr
	if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
	{
		close(serverSocket);
		return (-1);
	}

	// Turn serverSocket to listen mode
	if (listen(serverSocket, 128) == -1)
	{
		close(serverSocket);
		return (-1);
	}
	return (serverSocket);
}

// #1
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
