/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   11_print_find_get.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/18 08:46:58 by mondrew           #+#    #+#             */
/*   Updated: 2021/06/18 13:35:18 by mondrew          ###   ########.fr       */
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

void	printError(char	*msg)
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

int		findNewLine(char *str)
{
	int		i = 0;

	if (!msg)
		return (-1);
	while (str[i])
	{
		if (str[i] == '\n')
			return (i);
		i++;
	}
	return (-1);
}

char	*ft_getMsgWithId(const char *msg, int id)
{
	char	*newMsg = malloc(sizeof(char) * (strlen(msg) + 10 + 1));

	if (!newMsg)
		return (NULL);
	sprintf(newMsg, msg, id);
	return (newMsg);
}
