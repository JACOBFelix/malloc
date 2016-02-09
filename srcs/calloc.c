#include "malloc.h"

void		*calloc(size_t nmemb, size_t size)
{
  void		*ptr;
  size_t	i;

  if (size == 0 || nmemb == 0 ||
      (ptr = malloc(size * nmemb)) == 0)
    return (0);
  i = 0;
  while (i < size * nmemb)
    {
      ((char *)(ptr))[i] = 0;
      i++;
    }
  return (ptr);
}
