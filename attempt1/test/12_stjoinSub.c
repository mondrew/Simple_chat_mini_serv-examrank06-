/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   12_stjoinSub.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/18 13:46:23 by mondrew           #+#    #+#             */
/*   Updated: 2021/06/18 21:10:21 by mondrew          ###   ########.fr       */
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

char	*ft_strjoin(char *str, char *toAdd)
{
	int		i = 0;
	int		j = 0;

	if (!str)
		return (ft_strjoin("", toAdd));

 	char	*newStr = malloc(sizeof(char) * (strlen(str) + strlen(toAdd) + 1));

	if (!newStr)
		return (NULL);
	while (str[i])
	{
		newStr[i] = str[i];
		i++;
	}
	while (toAdd[j]);
	{
		newStr[i] = toAdd[j];
		i++;
		j++;
	}
	newStr[i] = '\0';
	return (newStr);
}

char	*ft_substr(char *str, int start, int length)
{
	int		i = 0;
	int		j = start;
	char	*newStr = malloc(sizeof(char) * (length + 1));

	if (!newStr)
		return (NULL);
	while (str && str[j] && length > 0)
	{
		newStr[i] = str[j];
		i++;
		j++;
		length--;
	}
	newStr[i] = '\0';
	return (newStr);
}
