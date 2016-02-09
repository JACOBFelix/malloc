#include <unistd.h>
#include "malloc.h"

void		*my_sbrk_add(size_t x, int page_size)
{
  if (x * page_size > DOUBLE_MAX &&
      (sbrk(DOUBLE_MAX) == (void *) -1 ||
       sbrk(x * page_size - DOUBLE_MAX) == (void *) -1))
    return ((void *) -1);
  else
    if (sbrk(DOUBLE_MAX - (x * page_size - DOUBLE_MAX)) == (void *) -1)
      return ((void *) -1);
  return ((void *) 0);
}

void		*my_sbrk_sub(size_t size)
{
  if (size > DOUBLE_MAX &&
      (sbrk(-(DOUBLE_MAX)) == (void *) -1 ||
       sbrk(-(size - DOUBLE_MAX)) == (void *) -1))
    return ((void *) -1);
  else
    if (sbrk(-size) == (void *) -1)
      return ((void *) -1);
  return (0);
}
