#include <unistd.h>
#include "malloc.h"

extern t_bloc_meta_data	*g_first_bloc;
extern t_bloc_meta_data	*g_last_bloc;
extern t_bloc_meta_data	*g_first_free;
extern pthread_mutex_t	g_free_mutex;

static void		*find_meta_data(void *ptr)
{
  if (ptr < (void *)g_first_bloc || ptr > (void *)g_last_bloc)
    return (0);
  while (ptr != g_first_bloc)
    {
      ptr--;
      if (is_inlist(ptr) == SUCCESS)
	return (ptr);
    }
  return (0);
}

size_t			malloc_usable_size(void *ptr)
{
  void			*tmp;

  if (ptr == 0)
    return (0);
  if ((tmp = find_meta_data(ptr)) == 0)
    return (0);
  return (((t_bloc_meta_data *)(tmp))->size);
}

int			malloc_trim(size_t pad)
{
  if (pad == 0)
    return (0);
  if (g_last_bloc && g_last_bloc->is_free == TRUE && g_last_bloc->size > pad)
    {
      if (sbrk(-pad) == (void *) -1)
	return (0);
      g_last_bloc->size -= pad;
      return (1);
    }
  return (0);
}
