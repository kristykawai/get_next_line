/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kchan <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/17 15:17:42 by kchan             #+#    #+#             */
/*   Updated: 2023/07/22 14:43:36 by kchan            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line_bonus.h"

void	free_ptr(char **ptr)
{
	free(*ptr);
	*ptr = NULL;
}

char	*extract_line(char **stash)
{
	char	*line;
	int		i;
	char	*temp_stash;

	i = 0;
	while ((*stash)[i] != '\0' && (*stash)[i] != '\n')
		i++;
	temp_stash = *stash;
	line = ft_substr(temp_stash, 0, i + 1);
	*stash = ft_strdup(&(*stash)[i + 1]);
	free_ptr(&temp_stash);
	return (line);
}

int	read_and_stash(int fd, char **buffer, char **stash)
{
	int		nbyte_read;
	char	*temp_stash;

	nbyte_read = 1;
	while (!ft_strchr(*stash, '\n') && nbyte_read > 0)
	{
		nbyte_read = read(fd, *buffer, BUFFER_SIZE);
		if (nbyte_read == -1)
			return (-1);
		(*buffer)[nbyte_read] = '\0';
		temp_stash = *stash;
		*stash = ft_strjoin(temp_stash, *buffer);
		free_ptr(&temp_stash);
	}
	return (nbyte_read);
}

char	*ft_getline(int fd, char **buffer, char **stash)
{
	int		total_nbyte_read;
	char	*partial_line;

	total_nbyte_read = read_and_stash(fd, buffer, stash);
	if ((total_nbyte_read == 0 || total_nbyte_read == -1) && !**stash)
	{
		free_ptr(stash);
		return (NULL);
	}
	if ((total_nbyte_read == -1) && **stash)
	{
		free_ptr(stash);
		return (NULL);
	}
	if (ft_strchr(*stash, '\n'))
		return (extract_line(stash));
	if (!ft_strchr(*stash, '\n') && (**stash))
	{
		partial_line = ft_strdup(*stash);
		free_ptr(stash);
		return (partial_line);
	}
	return (NULL);
}

char	*get_next_line(int fd)
{
	static char	*stash[OPEN_MAX + 1];
	char		*line;
	char		*buffer;

	if (fd < 0 || BUFFER_SIZE <= 0 || fd > OPEN_MAX)
		return (NULL);
	buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	if (!stash[fd])
		stash[fd] = ft_strdup("");
	line = ft_getline(fd, &buffer, &stash[fd]);
	free_ptr(&buffer);
	return (line);
}

// /*
// #include <fcntl.h>
// #include <stdio.h>
// int main()
// {
// 	int fd1;//az with no nl
// 	int fd2;//num with 3 nl 
// 	// int fd3; //empty file
// 	// int fd4; //article with 4 nls
// 	int fd5; //open error (files with wrong name)

// 	fd1 = open("test_az.txt", O_RDONLY);
// 	fd2 = open("test_article_4nl.txt", O_RDONLY);
// 	// fd3 = open("empty.txt", O_RDONLY);
// 	// fd4 = open("test_article_4nl.txt", O_RDONLY);
// 	// fd5 = open("non-exist.txt", O_RDONLY);
// 	// printf("%d",fd1);
// 	// printf("%d",fd2);
// 	// printf("%d",fd3);
// 	// printf("%d",fd4);
// 	// printf("%d",fd5); //return -1

// 	// printf("fd2:%p\n",get_next_line(fd2));
// 	// printf("fd2:%p\n",get_next_line(fd2));
// 	// printf("fd2:%p\n",get_next_line(fd2));
// 	// printf("fd2:%p\n",get_next_line(fd2));
// it free stash after the eof is readed 
// 	printf("first call fd2:%s\n",get_next_line(fd2));
// 	printf("second call fd2:%s\n",get_next_line(fd2));
// 		printf("3 call fd2:%s\n",get_next_line(fd2));
// 			printf("4 call fd2:%s\n",get_next_line(fd2));
// 					printf("5 call fd2:%s\n",get_next_line(fd2));
// 	// printf("%s",get_next_line(fd2));

// 	return(0);
// }
// */