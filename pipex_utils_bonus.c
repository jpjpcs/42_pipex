/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils_bonus.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joaosilva <joaosilva@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 12:19:03 by joaosilva         #+#    #+#             */
/*   Updated: 2024/03/26 20:29:12 by joaosilva        ###   ########.fr       */
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
		write(fd_here_doc, line, ft_strlen(line));
		write(fd_here_doc, "\n", 1);
		free(line);
		ft_printf("> ");
		line = get_next_line(0);
	}
	free(line);
	close(fd_here_doc);
}
