/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mondrew <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/17 08:56:35 by mondrew           #+#    #+#             */
/*   Updated: 2021/06/17 09:34:26 by mondrew          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

struct				s_client {
	int				id;
	int				socket;
	char			*msgToClient;
	char			*msgFromClient;
	int				deleteMe;
	struct s_client	*next;
}					t_client;

static int			number_of_clients = 0;
static const char	*arrivedMsg = "";
static const char	*leftMsg = "";
static const char	*nameMsg = "";
