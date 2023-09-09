#include "get_next_line.h"

//this is a function to return a line read from a file descriptor by a given buffer size 
//one line at a time calling the function
//return line include '\n' unless end of file was reached and does not end with '\n'
//Return NULL //when read == '\0' end of line or reading error

//static variable doesn't need malloc, it is like an additional memory storage during runtime
// #include <stdio.h>
size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i] != 0)
	{
		i++;
	}
	return (i);
}

char	*ft_strchr(const char *s, int c)
{
	while (*s)
	{
		if ((*s) == c)
			return ((char *)s);
		s++;
	}
	return (0);
}

char	*ft_strdup(const char *s1)
{
	char	*s2;
	size_t	i;

	i = 0;
	s2 = malloc(ft_strlen(s1) + 1);
	if (!s1)
		return (NULL);
	while (s1[i] != '\0')
	{
		s2[i] = s1[i];
	    i++;
	}
	s2[i] = '\0';
	return (s2);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	size_t	i;
	size_t	j;
	char	*sub;

	sub = (char *)malloc((sizeof(char)) * (len + 1));
	if (!sub)
		return (NULL);
	i = 0;
	j = 0;
	while (s[i] != '\0')
	{
		if (i >= start && j < len)
		{
			sub[j] = s[i];
			j++;
		}
		i++;
	}
	sub[j] = '\0';
	return (sub);
}

char	*ft_strjoin(char const *s1, char const *s2)
{
	size_t	i;
	size_t	j;
	char	*s3;

	s3 = (char *)malloc(ft_strlen(s1) + ft_strlen(s2) + 1);
	if (!s3)
		return (NULL);
	i = 0;
	j = 0;
	while (s1[i] != '\0')
	{
		s3[i] = s1[i];
		i++;
	}
	while (s2[j] != '\0')
	{
		s3[i + j] = s2[j];
		j++;
	}
	s3[i + j] = '\0';
	return (s3);
}

void	free_ptr(char **ptr)
{
	free(*ptr); //free the content value that this address point to 
	*ptr = NULL; // free address store inside the pointer
}

// #include <stdio.h>
char	*extract_line(char **stash)
{
	char	*line;
	int		i;
	char	*temp_stash;
	char	*trimmed_str;

	i = 0;
	while ((*stash)[i]!= '\0' && (*stash)[i] != '\n')
		i++;
	temp_stash = *stash; //copy the value of stash
	line = ft_substr(temp_stash, 0, i + 1); //new variable and address
	trimmed_str = ft_strdup(&(*stash)[i + 1]); 
	// printf("stash before free:%p\n", *stash);
	// free_ptr(&*stash); //free the old address of stored by *stash, which is the first position of the array sent to this function, either free temp_stash or this stash
	// printf("temp_stash:%s\n", temp_stash);
	// printf("stash after free:%p\n", *stash);
	*stash = trimmed_str; //shrink the stash to exclude extracted line,  assigns a new dynamically allocated string to *stash while skipping a specific position within the original string. t creates a pointer to the substring starting from position i + 1.
	// printf("trimmed str reassign:%p\n", trimmed_str);
	// printf("stash after reassign:%p\n", *stash); the first position of the subarray index stash sent to the function
	free_ptr(&temp_stash); //free memory of old stash address,the previous strjoined stash //set temp_stash to null
	return (line);
}

int	read_and_stash(int fd, char **buffer, char **stash) //continue reading until newline is found or eof as line size and buffer size are different, you need to do ft_strjoin if stash is not empty
{
	int		nbyte_read;
	char	*temp_stash;
	char	*join_str;
	
	nbyte_read = 1;
	while (!ft_strchr(*stash, '\n') && nbyte_read > 0)// keep reading until a newline is found
	{
		nbyte_read = read(fd, *buffer, BUFFER_SIZE); //if a new line is not found, it will keep calling the read function according to the buffer size
		if (nbyte_read == -1)
			return (-1);
		(*buffer)[nbyte_read] = '\0';
		temp_stash = *stash; //assign value pointed by stash
		join_str = ft_strjoin(temp_stash, *buffer);//as a new malloc is created each time looping, it won't have segmentation fault even buffer size is small than line size
		*stash = join_str;
		free_ptr(&temp_stash);
	}
	return (nbyte_read);
}

char	*getnewline(int fd, char **buffer, char **stash) //because passing the address of pointer from main
{
	int		total_nbyte_read;
	char	*partial_line;

	total_nbyte_read = read_and_stash(fd, buffer, stash);
	if ((total_nbyte_read == 0 || total_nbyte_read == -1) && !**stash) //stash is empty
	{
		free_ptr(stash);
		return (NULL);
	}
	if ((total_nbyte_read == -1) && **stash) //stash is not empty but reading error in the middle
	{
		free_ptr(stash);
		return (NULL);
	}
	if (ft_strchr(*stash, '\n')) //newline found
		return (extract_line(stash));
	if (!ft_strchr(*stash, '\n') && (**stash))//stash is not empty but no newline found 
	{
		partial_line = ft_strdup(*stash);
		free_ptr(stash);
		return (partial_line); //allocate memory to last line without new line
	}
	return (NULL);
}

char	*get_next_line(int fd) //each function for one file and one line at a time
{
	static char	*stash[OPEN_MAX + 1]; // declaraction of the static array, each index in the array can store constant character arrays
	char		*line;
	char		*buffer;

	if (fd < 0 || BUFFER_SIZE <= 0 || fd > OPEN_MAX)
		return (NULL);
	buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE + 1);
	if (!buffer)
		return (NULL);
	if (!stash[fd]) // to keep the index sequence until program finishes to avoid segmentation fault
		stash[fd] = ft_strdup(""); //alocate a seperate memory space of the error file
	line = getnewline(fd, &buffer, &stash[fd]);//taking the address of the element in the array 'stash' at index 'fd', without fd as index it will only refer to the first element
	free_ptr(&buffer);
	return (line);
}

// #include <fcntl.h>
// #include <stdio.h>
// int main()
// {
// 	// int fd1;//az with no nl
// 	int fd2;//num with 3 nl 
// 	// int fd3; //empty file
// 	// int fd4; //article with 4 nls
// 	// int fd5; //open error (files with wrong name)

// 	// fd1 = open("test_az.txt", O_RDONLY);
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
// 	// printf("fd2:%p\n",get_next_line(fd2));// it free stash after the eof is readed 

// 	printf("1 call fd2:%s\n",get_next_line(fd2));
// 	printf("2 call fd2:%s\n",get_next_line(fd2));
// 	printf("3 call fd2:%s\n",get_next_line(fd2));
// 	printf("4 call fd2:%s\n",get_next_line(fd2));
// 	printf("5 call fd2:%s\n",get_next_line(fd2));
// 	// printf("%s",get_next_line(fd2));

// 	return(0);
// }