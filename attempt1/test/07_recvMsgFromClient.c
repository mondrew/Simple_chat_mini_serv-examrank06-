/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   7_recvMsgFromClient.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 15:31:39 by mondrew           #+#    #+#             */
/*   Updated: 2021/06/17 15:57:40 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

int		recvMsgFromClient(t_client *client, t_client **clientNode)
{
	char	buf[1024];

	memset(buf, 0, 1024);

	int		ret = read(client->socket, buf, 1023);

	if (ret == -1)
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
		char	*newStr = ft_strjoin(client->msgFromClient, buf);
		if (!newStr)
			return (-1);
		free(client->msgFromClient);
		client->msgFromClient = newStr;
	}
	return (0);
}
