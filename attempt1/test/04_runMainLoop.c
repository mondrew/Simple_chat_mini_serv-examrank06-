/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   4_runMainLoop.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 10:44:17 by mondrew           #+#    #+#             */
/*   Updated: 2021/06/17 12:05:41 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet.h>
#include <errno.h>

int		runMainLoop(int serverSocket)
{
	t_client	*clientNode = NULL;
	fd_set		rds;
	fd_set		wrs;
	int			maxFd = -1;

	while (1)
	{
		// Reset fd sets
		FD_ZERO(&rds);
		FD_ZERO(&wrs);

		// Find maxFd
		maxFd = findMaxFd(serverSocket, clientNode);

		// Add sockets to sets
		FD_SET(serverSocket, &rds);

		t_client	*tmp = clientNode;
		while (tmp)
		{
			FD_SET(tmp->socket, &rds);
			FD_SET(tmp->socket, &wrs);
			tmp = tmp->next;
		}

		// Select
		int ret = select(maxFd + 1, &rds, &wrs, 0, 0);
		if (ret < 0)
		{
			if (errno == EINTR)
				continue ;
			else
			{
				freeClients(clientNode); // &?
				return (-1);
			}
		}
		else if (ret == 0)
			continue ;
		else
		{
			// Check server socket
			if (FD_ISSET(serverSocket, &rds))
			{
				if (acceptClient(serverSocket, &clientNode) == -1)
				{
					freeClients(clientNode);
					return (-1);
				}
			}

			// Check clients' sockets
			t_client	*tmp = clientNode;
			while (tmp)
			{
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
