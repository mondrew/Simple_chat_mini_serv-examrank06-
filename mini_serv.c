/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/14 20:18:45 by mondrew           #+#    #+#             */
/*   Updated: 2021/06/16 23:09:56 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
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

static int	number_of_clients = 0;
static const char	*arrivedMsg = "server: client %d just arrived\n";
static const char	*leftMsg = "server: client %d just left\n";
static const char	*nameMsg = "client %d: ";

char	*ft_strjoin(char *str, char *toAdd)
{
	int		i = 0;
	int		j = 0;

	if (!str)
		return (ft_strjoin("", toAdd));

	char	*newStr = malloc(sizeof(char) * (strlen(str) + strlen(toAdd) + 1));

	if (newStr == NULL)
		return (NULL);
	while (str[i])
	{
		newStr[i] = str[i];
		i++;
	}
	while (toAdd[j])
	{
		newStr[i] = toAdd[j];
		i++;
		j++;
	}
	newStr[i] = '\0';
	return (newStr);
}

void	deleteClient(t_client *client, t_client **clientNode)
{
	t_client	*tmp = *clientNode;

	// Check if client is the 1st element in the list
	if (tmp == client)
	{
		tmp = (*clientNode)->next;
		close(client->socket);
		if (client->msgToClient)
			free(client->msgToClient);
		if (client->msgFromClient)
			free(client->msgFromClient);
		free(client);
		*clientNode = tmp;
	}
	else
	{
		while (tmp && tmp->next != client)
			tmp = tmp->next;
		tmp->next = client->next;
		close(client->socket);
		if (client->msgToClient)
			free(client->msgToClient);
		if (client->msgFromClient)
			free(client->msgFromClient);
		free(client);
	}
}

void	printError(char *errorMsg)
{
	write(2, errorMsg, strlen(errorMsg));
}

int		findMaxFd(int serverSocket, t_client *clientNode)
{
	int max = serverSocket;

	while (clientNode)
	{
		if (clientNode->socket > max)
			max = clientNode->socket;
		clientNode = clientNode->next;
	}
	return (max);
}

int		findNewLine(char *str)
{
	if (!str)
		return (-1);
	for (int i = 0; str[i]; i++)
	{
		if (str[i] == '\n')
			return (i);
	}
	return (-1);
}

char	*ft_substr(char *str, int start, int length)
{
	char	*newStr = malloc(sizeof(char) * (length + 1));
	int		i = 0;
	int		j = start;

	if (newStr == NULL)
		return (NULL);
	while (str && str[j] && length > 0)
	{
		newStr[i] = str[j];
		i++;
		j++;
		length--;
	}
	newStr[i] = '\0';
	return (newStr);
}

char	*ft_getMsgWithId(const char *str, int id)
{
	char	*ret = malloc(sizeof(char) * (strlen(str) + 10 + 1));

	if (!ret)
		return (NULL);
	sprintf(ret, str, id);
	return (ret);
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

int		sendAll(char *str, t_client **clientNode, t_client *except)
{
	t_client	*tmp = *clientNode;
	char		*newMsg = NULL;

	while (tmp)
	{
		if (tmp == except)
		{
			tmp = tmp->next;
			continue ;
		}
		if (!tmp->msgToClient)
			newMsg = ft_strjoin("", str);
		else
			newMsg = ft_strjoin(tmp->msgToClient, str);
		if (!newMsg)
			return (-1);
		free(tmp->msgToClient);
		tmp->msgToClient = newMsg;

		tmp = tmp->next;
	}
	return (0);
}

int		acceptClient(int serverSocket, t_client **clientNode)
{
	int					clientSocket;
	struct sockaddr_in	addr;
	socklen_t			len = sizeof(addr);
	t_client			*newClient;

	if ((clientSocket = accept(serverSocket, (struct sockaddr *)&addr, &len)) == -1)
		return (-1);

	if ((newClient = malloc(sizeof(t_client))) == NULL)
		return (-1);

	newClient->id = number_of_clients;
	number_of_clients++;
	newClient->socket = clientSocket;
	newClient->msgToClient = NULL;
	newClient->msgFromClient = NULL;
	newClient->deleteMe = 0;
	newClient->next = NULL;

	// Add new Client to the Clients list
	if (*clientNode == NULL)
		*clientNode = newClient;
	else
	{
		t_client	*tmp = *clientNode;
		while (tmp->next != NULL)
			tmp = tmp->next;
		tmp->next = newClient;
	}

	// SendAll 'Arrived msg' except newClient
	char	*newClientMsg = ft_getMsgWithId(arrivedMsg, newClient->id);
	if (!newClientMsg)
		return (-1);
	if (sendAll(newClientMsg, clientNode, newClient) == -1)
	{
		free(newClientMsg);
		return (-1);
	}
	free(newClientMsg);
	return (0);
}

int		sendMsgToClient(t_client *client)
{
	int i = findNewLine(client->msgToClient);

	if (i == -1)
		return (0);
	int ret = write(client->socket, client->msgToClient, i + 1);
	if (ret == -1)
		return (-1);

	char	*tmp = ft_substr(client->msgToClient, ret, strlen(client->msgToClient) - ret);
	if (!tmp)
		return (-1);
	free(client->msgToClient);
	client->msgToClient = tmp;
	return (0);
}

int		recvMsgFromClient(t_client *client, t_client **clientNode)
{
	char	buf[1024];
	int		ret;

	memset(buf, 0, 1024);
	if ((ret = read(client->socket, buf, 1023)) == -1)
		return (-1);
	else if (ret == 0)
	{
		char	*leftMsgWithId = ft_getMsgWithId(leftMsg, client->id);
		if (!leftMsgWithId)
			return (-1);
		if (sendAll(leftMsgWithId, clientNode, client) == -1)
		{
			free(leftMsgWithId);
			return (-1);
		}
		free(leftMsgWithId);
		client->deleteMe = 1;
	}
	else
	{
		buf[ret] = '\0';
		char	*newStr = ft_strjoin(client->msgFromClient, buf);
		if (!newStr)
			return (-1);
		free(client->msgFromClient);
		client->msgFromClient = newStr;
	}
	return (0);
}

int		resendMessages(t_client *client, t_client **clientNode)
{
	// Find all newlines and resend them to others
	int		i;
	while ((i = findNewLine(client->msgFromClient)) != -1)
	{
		char	*rawMsg = ft_substr(client->msgFromClient, 0, i + 1);
		if (!rawMsg)
			return (-1);
		char	*nameMsgWithId = ft_getMsgWithId(nameMsg, client->id);
		if (!nameMsgWithId)
		{
			free(rawMsg);
			return (-1);
		}
		char	*message = ft_strjoin(nameMsgWithId, rawMsg);
		if (!message)
		{
			free(rawMsg);
			free(nameMsgWithId);
			return (-1);
		}
		free(nameMsgWithId);
		free(rawMsg);

		char	*tmp = ft_substr(client->msgFromClient, i + 1, strlen(client->msgFromClient) - (i + 1));
		if (!tmp)
		{
			free(message);
			return (-1);
		}
		free(client->msgFromClient);
		client->msgFromClient = tmp;

		if (sendAll(message, clientNode, NULL) == -1)
		{
			free(message);
			return (-1);
		}
		free(message);
	}
	return (0);
}

int		runMainLoop(int serverSocket)
{
	t_client	*clientNode = NULL;
	fd_set		rds;
	fd_set		wrs;
	int			maxFd = -1;

	while (1)
	{
		FD_ZERO(&rds);
		FD_ZERO(&wrs);
		maxFd = findMaxFd(serverSocket, clientNode);

		FD_SET(serverSocket, &rds);
		t_client	*tmp = clientNode;
		while (tmp)
		{
			FD_SET(tmp->socket, &rds);
			FD_SET(tmp->socket, &wrs);
			tmp = tmp->next;
		}

		int res = select(maxFd + 1, &rds, &wrs, 0, 0);
		if (res < 0)
		{
			if (errno == EINTR)
				continue ;
			else
			{
				freeClients(clientNode);
				return (-1);
			}
		}
		else if (res == 0)
			continue ;
		else
		{
			// Check if serverSocket has new incoming connections
			if (FD_ISSET(serverSocket, &rds))
			{
				if (acceptClient(serverSocket, &clientNode) == -1)
				{
					freeClients(clientNode);
					return (-1);
				}
			}
			// Check if Clients want to Read or Write msgs
			t_client	*tmp = clientNode;
			while (tmp)
			{
				// Important!!!
				if (tmp->msgToClient != NULL && strlen(tmp->msgToClient) && \
													FD_ISSET(tmp->socket, &wrs))
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
	int					serverSocket;
	struct sockaddr_in	serverAddr;

	// Creating the listening server socket
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
		return (-1);

	// Set serverAddr fields before 'bind'
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1

	// Binding the socket 'serverSocket' to IP address
	if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
	{
		close(serverSocket);
		return (-1);
	}

	// Turn server socket to the listening mode
	if (listen(serverSocket, 128) == -1)
	{
		close(serverSocket);
		return (-1);
	}
	return (serverSocket);
}

int		main(int argc, char **argv)
{
	int					serverSocket;
	int					port;

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
		close(serverSocket);
		printError("Fatal error\n");
		return (1);
	}
	return (0);
}
