#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int		maxFd = 0;
int		numberOfClients = 0;
int		id_by_sock[65536];

fd_set	ready_for_read, ready_for_write, active_sockets;
char	buf_for_read[42 * 4096], buf_str[42 * 4096], buf_for_write[42 * 4096 + 42];

void	sendAll(int except)
{
	for (int sock = 0; sock <= maxFd; sock++)
	{
		if (FD_ISSET(sock, &ready_for_write) && sock != except)
			send(sock, buf_for_write, strlen(buf_for_write), 0);
	}
}

void	fatalError(void)
{
	write(2, "Fatal error\n", strlen("Fatal error\n"));
	exit(1);
}

int		main(int argc, char **argv)
{
	if (argc != 2)
	{
		write(2, "Wrong number of arguments\n", strlen("Wrong number of arguments\n"));
		exit(1);
	}
	uint16_t	port = atoi(argv[1]);
	int			serverSocket = -1;
	struct sockaddr_in	serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = (1 << 24) | 127;
	serverAddr.sin_port = (port >> 8) | (port << 8);

	// start server
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		fatalError();
	if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
	{
		close(serverSocket);
		fatalError();
	}
	if (listen(serverSocket, 128) == -1)
	{
		close(serverSocket);
		fatalError();
	}
	// server launched
	bzero(id_by_sock, sizeof(id_by_sock));
	FD_ZERO(&active_sockets);
	FD_SET(serverSocket, &active_sockets);
	maxFd = serverSocket;

	// main loop
	while (1)
	{
		ready_for_read = ready_for_write = active_sockets;
		if (select(maxFd + 1, &ready_for_read, &ready_for_write, 0, 0) <= 0)
			continue ;
		for (int sock = 0; sock <= maxFd; sock++)
		{
			if (FD_ISSET(sock, &ready_for_read))
			{
				if (sock == serverSocket)
				{
					struct sockaddr_in	clientAddr;
					socklen_t			len = sizeof(clientAddr);
					int	clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &len);
					if (clientSocket == -1)
						continue ;

					// add client to array
					FD_SET(clientSocket, &active_sockets);
					id_by_sock[clientSocket] = numberOfClients++;
					maxFd = clientSocket > maxFd ? clientSocket : maxFd;

					// send all arrived msg
					sprintf(buf_for_write, "server: client %d just arrived\n", \
																	id_by_sock[clientSocket]);
					sendAll(clientSocket);
					break ;
				}
				else
				{
					int		ret = recv(sock, buf_for_read, 42 * 4096, 0);
					if (ret <= 0)
					{
						sprintf(buf_for_write, "server: client %d just left\n", id_by_sock[sock]);
						sendAll(sock);
						FD_CLR(sock, &active_sockets);
						close(sock);
						break ;
					}
					else
					{
						for (int i = 0, j = 0; i < ret; i++, j++)
						{
							buf_str[j] = buf_for_read[i];
							if (buf_str[j] == '\n')
							{
								buf_str[j] = '\0';
								sprintf(buf_for_write, "client %d: %s\n", id_by_sock[sock], buf_str);
								sendAll(sock);
								j = -1;
							}
						}
					}
				}
			}
		}
	}
	return (0);
}
