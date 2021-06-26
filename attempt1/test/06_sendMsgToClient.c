/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   6_sendMsgToClient.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 15:10:36 by mondrew           #+#    #+#             */
/*   Updated: 2021/06/17 22:38:38 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet.h>
#include <errno.h>

int		sendMsgToClient(t_client *client)
{
	// Find if there is newline in msgToClient
	int		i = findNewLine(client->msgToClient);

	if (i == -1)
		return (0);

	int		ret = write(client->socket, client->msgToClient, i + 1);
	if (ret == -1)
		return (-1);

	// Trancate original line
	char	*newStr = ft_substr(client->msgToClient, ret, strlen(client->msgToClient) - ret);
	if (!newStr)
		return (-1);
	free(client->msgToClient);
	client->msgToClient = newStr;
	return (0);
}
