/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jode-jes <jode-jes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 12:19:03 by joaosilva         #+#    #+#             */
/*   Updated: 2024/03/27 18:55:24 by jode-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex_bonus.h"

void	ft_here_doc(char *eof)
{
	char	*line;
	int		fd_here_doc;

	fd_here_doc = open("here_doc", O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (fd_here_doc == -1)
	{
		perror("Error");
		exit(1);
	}
	ft_printf("> ");
	line = get_next_line(0);
	while (line && (ft_strncmp(line, eof, ft_strlen(eof)) || ft_strclen(line,
				'\n') != ft_strlen(eof)))
	{
		ft_putstr_fd(line, fd_here_doc);
		free(line);
		ft_printf("> ");
		line = get_next_line(0);
	}
	free(line);
	close(fd_here_doc);
}
