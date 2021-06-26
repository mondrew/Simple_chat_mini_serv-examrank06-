/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   10_free_delete.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/18 07:57:59 by mondrew           #+#    #+#             */
/*   Updated: 2021/06/18 08:46:18 by mondrew          ###   ########.fr       */
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

void	deleteDisconnectedClient(t_client **clientNode)
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

void	deleteClient(t_client *del, t_client **clientNode)
{
	t_client	*tmp = *clientNode;

	if (tmp == del)
	{
		// First element
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
