/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   5_acceptClient.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 12:44:52 by mondrew           #+#    #+#             */
/*   Updated: 2021/06/17 22:45:11 by mondrew          ###   ########.fr       */
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

int		acceptClient(int serverSocket, t_client **clientNode)
{
	t_client			*newClient = NULL;
	struct sockaddr_in	addr;
	socklen_t			len = sizeof(addr);
	int					clientSocket;

	// Accept new connection
	clientSocket = accept(serverSocket, (struct sockaddr *)&addr, &len);
	if (clientSocket == -1)
		return (-1);

	// Create new client
	if (!(newClient = malloc(sizeof(t_client))))
	{
		close(clientSocket);
		return (-1);
	}

	// Initialize new client
	newClient->id = number_of_clients;
	number_of_clients++;
	newClient->socket = clientSocket;
	newClient->msgToClient = ft_strjoin("", "");
	if (!newClient->msgToClient)
	{
		close(clientSocket);
		free(newClient);
		return (-1);
	}
	newClient->msgFromClient = ft_strjoin("", "");
	if (!newClient->msgFromClient)
	{
		close(clientSocket);
		free(newClient->msgToClient);
		free(newClient);
		return (-1);
	}
	newClient->deleteMe = 0;
	newClient->next = NULL;

	// Add new Client to the Client list
	if (!(*clientNode))
		*clientNode = newClient;
	else
	{
		t_client	tmp = *clientNode;
		while (tmp->next != NULL)
			tmp = tmp->next;
		tmp->next = newClient;
	}

	// SentAll 'arrived' msg
	char	*newClientMsg = getMsgWithId(arrivedMsg, newClient->id);
	if (newClientMsg == -1)
		return (-1);
	if (sendAll(newClientMsg, clientNode, newClient) == -1)
	{
		free(newClientMsg);
		return (-1);
	}
	free(newClientMsg);
	return (0);
}
