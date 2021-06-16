/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/16 11:19:17 by mondrew           #+#    #+#             */
/*   Updated: 2021/06/16 12:56:16 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int		main(void)
{
	char	*str = malloc(13);
	strcpy(str, "Hello\nWorld!");
	int		i = 5;

	printf("strlen: %ld\n", strlen(str));
	strcpy(str, str + i + 1);
	char	*ret = realloc(str, strlen(str));
	if (!ret)
		return (1);
	printf("strlen: %ld\n", strlen(str));

	return (0);
}
