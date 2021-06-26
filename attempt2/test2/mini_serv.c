#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

char	*nameMsg = "client %d: ";
char	*arrivedMsg = "server: client %d just arrived\n";
char	*leftMsg = "server: client %d just left\n";
int		sendAllFlag = 0;
fd_set	rds;
fd_set	wrs;
int		numberOfClients = 0;

typedef struct		s_client {
	int				id;
	int				socket;
	char			*msg;
	struct s_client	*next;
}					t_client;

int extract_message(char **buf, char **msg)
{
	char	*newbuf;
	int	i;

	*msg = 0;
	if (*buf == 0)
		return (0);
	i = 0;
	while ((*buf)[i])
	{
		if ((*buf)[i] == '\n')
		{
			newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
			if (newbuf == 0)
				return (-1);
			strcpy(newbuf, *buf + i + 1);
			*msg = *buf;
			(*msg)[i + 1] = 0;
			*buf = newbuf;
			return (1);
		}
		i++;
	}
	return (0);
}

char *str_join(char *buf, char *add)
{
	char	*newbuf;
	int		len;

	if (buf == 0)
		len = 0;
	else
		len = strlen(buf);
	newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
	if (newbuf == 0)
		return (0);
	newbuf[0] = 0;
	if (buf != 0)
		strcat(newbuf, buf);
	// free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

void	printError(char *msg)
{
	write(2, msg, strlen(msg));
}

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

char	*getMsgWithId(char *msg, int id)
{
	char	*newMsg = malloc(sizeof(char) * (strlen(msg) + 11));

	if (!newMsg)
		return (NULL);
	sprintf(newMsg, msg, id);
	return (newMsg);
}

void	deleteClient(t_client *del, t_client **clientNode)
{
	t_client	*tmp = *clientNode;

	if (!(*clientNode) || !del)
		return ;
	if (*clientNode == del)
	{
		*clientNode = (*clientNode)->next;
		close(del->socket);
		if (del->msg)
			free(del->msg);
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
		if (del->msg)
			free(del->msg);
		free(del);
	}
}

void	freeClients(t_client *clientNode)
{
	t_client	*tmp = clientNode;

	while (clientNode)
	{
		tmp = clientNode;
		clientNode = clientNode->next;
		deleteClient(tmp, &clientNode);
	}
}

int		sendAll(char *msg, t_client **clientNode, t_client *except)
{
	t_client	*tmp = *clientNode;

	while (tmp)
	{
		if (tmp != except && FD_ISSET(tmp->socket, &wrs))
		{
			send(tmp->socket, msg, strlen(msg), 0);
			sendAllFlag = 1;
		}
		tmp = tmp->next;
	}
	return (0);
}

int		recvMsgFromClient(t_client *client, t_client **clientNode)
{
	char	buffer[65536];

	memset(buffer, 0, 65536);

	int ret = recv(client->socket, buffer, 65535, 0);
	if (ret < 0)
		return (0);
	else if (ret == 0)
	{
		char	*newLeftMsg = getMsgWithId(leftMsg, client->id);
		if (!newLeftMsg)
			return (-1);
		sendAll(newLeftMsg, clientNode, client);
		free(newLeftMsg);
		deleteClient(client, clientNode);
		return (0);
	}
	else
	{
		buffer[ret] = '\0';
		char	*oldMsg = client->msg;
		if (!(client->msg = str_join(client->msg, buffer)))
		{
			if (oldMsg)
				free(oldMsg);
			return (-1);
		}
		if (oldMsg)
			free(oldMsg);
		char	*msgToSend = NULL;
		int		res = extract_message(&client->msg, &msgToSend);
		if (res == -1)
			return (-1);
		else if (res == 0)
			return (0);
		else
		{
			char	*newNameMsg = getMsgWithId(nameMsg, client->id);
			if (!newNameMsg)
			{
				free(msgToSend);
				return (-1);
			}
			char	*fullMsg = str_join(newNameMsg, msgToSend);
			if (!fullMsg)
			{
				free(msgToSend);
				free(newNameMsg);
				return (-1);
			}
			free(msgToSend);
			free(newNameMsg);

			sendAll(fullMsg, clientNode, client);

			free(fullMsg);
		}
	}
	return (0);
}

int		acceptClient(int serverSocket, t_client **clientNode)
{
	struct sockaddr_in	clientAddr;
	socklen_t			len = sizeof(clientAddr);
	int					clientSocket;
	t_client			*newClient;

   	if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &len)) == -1)
		return (0);
	if (!(newClient = malloc(sizeof(t_client))))
	{
		close(clientSocket);
		return (-1);
	}
	newClient->id = numberOfClients++;
	newClient->socket = clientSocket;
	newClient->msg = NULL;
	newClient->next = NULL;

	if (!(*clientNode))
		*clientNode = newClient;
	else
	{
		t_client	*tmp = *clientNode;
		while(tmp->next)
			tmp = tmp->next;
		tmp->next = newClient;
	}

	char	*newArrivedMsg = getMsgWithId(arrivedMsg, newClient->id);
	if (!newArrivedMsg)
		return (-1);
	sendAll(newArrivedMsg, clientNode, newClient);
	free(newArrivedMsg);
	return (0);
}

int		runMainLoop(int serverSocket)
{
	t_client	*clientNode = NULL;
	int			maxFd = serverSocket;

	while (1)
	{
		sendAllFlag = 0;
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

		int		ret = select(maxFd + 1, &rds, &wrs, 0, 0);

		if (ret	<= 0)
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
			t_client	*cli = clientNode;
			while (cli)
			{
				t_client	*tmp = cli;
				cli = cli->next;
				if (!sendAllFlag && FD_ISSET(tmp->socket, &rds))
				{
					if (recvMsgFromClient(tmp, &clientNode) == -1)
					{
						freeClients(clientNode);
						return (-1);
					}
				}
			}
		}
	}
	return (0);
}

int		startServer(int port)
{
	struct sockaddr_in	serverAddr;
	int		serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (serverSocket == -1)
		return (-1);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(2130706433);
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
	if (argc != 2)
	{
		printError("Wrong number of arguments\n");
		return (1);
	}
	int		port = atoi(argv[1]);
	int		serverSocket;
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
