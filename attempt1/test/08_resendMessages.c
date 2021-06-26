/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   8_resendMessages.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 15:57:56 by mondrew           #+#    #+#             */
/*   Updated: 2021/06/17 22:23:32 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

int		resendMessages(t_client *client, t_client **clientNode)
{
	int		i;

	while ((i = findNewLine(client->msgFromClient)) != -1)
	{
		// Get fullLine = nameLine+rawLine
		char	*nameLine = ft_getMsgWithId(nameMsg, client->id);
		if (!nameLine)
			return (-1);
		char	*rawLine = ft_substr(client->msgFromClient, 0, i + 1);
		if (!rawLine)
		{
			free(nameLine);
			return (-1);
		}
		char	*fullLine = ft_strjoin(nameLine, rawLine);
		if (!fullLine)
		{
			free(nameLine);
			free(rawLine);
			return (-1);
		}
		free(nameLine);
		free(rawLine);

		// Trancate original line
		char	*newStr = ft_substr(client->msgFromClient, i + 1, strlen(client->msgFromClient) - (i + 1));
		if (!newStr)
		{
			free(fullLine);
			return (-1);
		}
		free(client->msgFromClient);
		client->msgFromClient = newStr;

		if (sendAll(fullLine, clientNode, NULL) == -1)
		{
			free(fullLine);
			return (-1);
		}
		free(fullLine);
	}
	return (0);
}
