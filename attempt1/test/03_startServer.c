/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   3_startServer.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 09:35:01 by mondrew           #+#    #+#             */
/*   Updated: 2021/06/17 10:43:58 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>

int		startServer(int port)
{
	int					serverSocket;
	struct sockaddr_in	serverAddr;

	// Create server socket
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1)
		return (-1);

	// Fill sockaddr_in struct
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(2130706433); // 127.0.0.1
	serverAddr.sin_port = htons(port);

	// Bind socket to addr
	if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
	{
		close(serverSocket);
		return (-1);
	}

	// Turn socket to listen mode
	if (listen(serverSocket, 128) == -1)
	{
		close(serverSocket);
		return (-1);
	}
	return (serverSocket);
}
