#ifndef GET_NEXT_LINE_BONUS_H
# define GET_NEXT_LINE_BONUS_H

# include <stdlib.h>
# include <unistd.h>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 42
# endif

# ifndef FD_MAX
#  define FD_MAX 1024
# endif

char    *get_next_line(int fd);
size_t  ft_strlen(const char *s);
char    *ft_strchr(const char *s, int c);
char    *ft_strjoin(char *s1, char *s2);

#endif