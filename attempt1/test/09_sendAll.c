/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   9_sendAll.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 22:25:44 by mondrew           #+#    #+#             */
/*   Updated: 2021/06/17 23:00:15 by mondrew          ###   ########.fr       */
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

int		sendAll(char *msg, t_client **clientNode, t_client *except)
{
	t_client	*tmp = *clientNode;

	while (tmp)
	{
		if (tmp == except)
		{
			tmp = tmp->next;
			continue ;
		}
		char	*str;
		if (!tmp->msgToClient)
			newStr = ft_strjoin("", msg);
		else
			newStr = ft_strjoin(tmp->msgToClient, msg);
		if (!newStr)
			return (-1);
		if (tmp->msgToClient)
			free(tmp->msgToClient);
		tmp->msgToClient = newStr;
		tmp = tmp->next;
	}
	return (0);
}
