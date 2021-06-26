/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   2_main.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 09:22:11 by mondrew           #+#    #+#             */
/*   Updated: 2021/06/17 09:34:14 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

int		main(int argc, char **argv)
{
	int		serverSocket;
	int		port;

	if (argc != 2)
	{
		printError("Wrong number of arguments\n");
		return (1);
	}
	port = atoi(argv[1]);
	if ((serverSocket = startServer(port)) == -1)
	{
		printError("Fatal error\n");
		return (1);
	}
	if (runMainLoop(serverSocket) == -1)
	{
		close(serverSocket);
		printError("Fatal error\n");
		return (1);
	}
	return (0);
}
