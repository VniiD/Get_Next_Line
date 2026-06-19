#include "get_next_line_bonus.h"

static char	*read_to_stash(int fd, char *stash)
{
	char	*buffer;
	ssize_t	bytes_read;

	buffer = (char *)malloc((BUFFER_SIZE + 1) * sizeof(char));
	if (!buffer)
		return (free(stash), NULL);
	bytes_read = 1;
	while (!ft_strchr(stash, '\n') && bytes_read != 0)
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read == -1)
		{
			free(buffer);
			return (free(stash), NULL);
		}
		buffer[bytes_read] = '\0';
		stash = ft_strjoin(stash, buffer);
	}
	free(buffer);
	return (stash);
}

static char	*get_line(char *stash)
{
	int		i;
	char	*str;

	i = 0;
	if (!stash[i])
		return (NULL);
	while (stash[i] && stash[i] != '\n')
		i++;
	str = (char *)malloc(i + 2);
	if (!str)
		return (NULL);
	i = -1;
	while (stash[++i] && stash[i] != '\n')
		str[i] = stash[i];
	if (stash[i] == '\n')
		str[i++] = '\n';
	str[i] = '\0';
	return (str);
}

static char	*clean_stash(char *stash)
{
	int		i;
	int		j;
	char	*str;

	i = 0;
	while (stash[i] && stash[i] != '\n')
		i++;
	if (!stash[i] || !stash[i + 1])
		return (free(stash), NULL);
	str = (char *)malloc(ft_strlen(stash) - i);
	if (!str)
		return (free(stash), NULL);
	j = 0;
	while (stash[++i])
		str[j++] = stash[i];
	str[j] = '\0';
	free(stash);
	return (str);
}

char	*get_next_line(int fd)
{
	static char	*stash[FD_MAX];
	char		*line;

	if (fd < 0 || BUFFER_SIZE <= 0 || fd >= FD_MAX)
		return (NULL);
	stash[fd] = read_to_stash(fd, stash[fd]);
	if (!stash[fd])
		return (NULL);
	line = get_line(stash[fd]);
	stash[fd] = clean_stash(stash[fd]);
	return (line);
}
