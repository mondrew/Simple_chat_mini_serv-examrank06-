#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct 	s_client {
	int				id;
	int				socket;
	char			*msg;
	struct s_client	*next;
}				t_client;

// Extracts newline from buf to msg and moves the buf
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
	free(buf);
	strcat(newbuf, add);
	return (newbuf);
}

char	buffer[65536];
char	*leftMsg = "";
char	*arrivedMsg = "";
char	*nameMsg = "";
fd_set	rds;
fd_set	wrs;
int		sendToAllFlag = 0;

char	*sendAll(char *msg, t_client *clientNode, t_client *except)
{
	t_client	*tmp = clientNode;

	while (tmp)
	{
		if (tmp != except && FD_ISSET(tmp->socket, &wrs))
		{
			int		ret = send(tmp->socket, msg, sizeof(msg), 0);
			if (ret <= 0)
			{
				tmp = tmp->next;
				continue ;
			}
			else
			{
				if (ret < sizeof(msg))
				{
					// TODO
				}
			}
		}
		tmp = tmp->next;
	}
	sendToAllFlag = 1;
	return (NULL);
}

int		recvMsgFromClient(t_client *client, t_client **clientNode)
{
	char	buf[65536];

	memset(buf, 0, 65536);
	int		ret = recv(client->socket, buf, 65536, 0);

	if (ret <= 0)
	{
		// finish, close
	}
	else
	{
		buf[ret] = '\0';
		char	oldMsg = client->msg;
		if (!(client->msg = str_join(oldMsg, buf)))
		{
			if (oldMsg)
				free(oldMsg);
			return (-1);
		}
		if (oldMsg)
			free(oldMsg);
	}
	// RESEND ALL
	char	*msg = NULL;
	int		res = extract_message(&client->msg, &msg);
	if (res == 0)
	{
	}
	free(msg); // !!! don't forget!!!
}

int		runMainLoop(int serverSocket)
{
	t_client	*clientNode = NULL;
	int			maxFd = serverSocket;

	while (1)
	{
		FD_ZERO(&rds);
		FD_ZERO(&wrs);
		maxFd = findMaxFd(serverSocket, clientNode);
		sendToAllFlag = 0;

		FD_SET(serverSocket, &rds);
		t_client	*tmp = clientNode;
		while (tmp)
		{
			FD_SET(tmp->socket, &rds);
			FD_SET(tmp->socket, &wrs);
			tmp = tmp->next;
		}

		int		ret = select(maxFd + 1, &rds, &wrs, 0, 0);
		if (ret <= 0)
			continue ;
		else
		{
			if (FD_ISSET(serverSocket, &rds))
			{
				if (acceptClient(&clientNode) == -1)
				{
					freeClients(clientNode);
					return (-1);
				}
				continue ;
			}
			else
			{
				t_client	*tmp = clientNode;
				while (tmp)
				{
					t_client	*cli = tmp;
					tmp = tmp->next;
					if (FD_ISSET(cli->socket, &rds))
					{
						// Client wants to write smth
						if (recvMsgFromClient(cli, &clientNode) == -1)
						{
							freeClients(clientNode);
							return (-1);
						}
					}
				}
			}
		}
	}
	return (0);
}

int		startError(int port)
{
	struct sockaddr_in	serverAddr;
	int					serverSocket;

	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return (-1);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(2130706433);
	serverAddr.sin_port = htols(port);

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

int		main(int argc, char **argc)
{
	int		port;
	int		serverSocket;

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


int		main(int argc, char **argv) {

	int					sockfd, connfd, len;
	struct sockaddr_in	servaddr, cli; 

	// socket create and verification 
	sockfd = socket(AF_INET, SOCK_STREAM, 0); 
	if (sockfd == -1)
	{ 
		printf("socket creation failed...\n");
		exit(0);
	} 
	else
		printf("Socket successfully created..\n");
	bzero(&servaddr, sizeof(servaddr)); 

	// assign IP, PORT 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_addr.s_addr = htonl(2130706433); //127.0.0.1
	servaddr.sin_port = htons(8081); 
  
	// Binding newly created socket to given IP and verification 
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) { 
		printf("socket bind failed...\n"); 
		exit(0); 
	} 
	else
		printf("Socket successfully binded..\n");
	if (listen(sockfd, 10) != 0) {
		printf("cannot listen\n"); 
		exit(0); 
	}
	len = sizeof(cli);
	connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
	if (connfd < 0) { 
        printf("server acccept failed...\n"); 
        exit(0); 
    } 
    else
        printf("server acccept the client...\n");
}
