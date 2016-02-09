#include <unistd.h>
#include "malloc.h"

t_bloc_meta_data	*g_first_bloc = 0;
t_bloc_meta_data        *g_last_bloc = 0;
t_bloc_meta_data	*g_first_free = 0;
pthread_mutex_t		g_free_mutex = PTHREAD_MUTEX_INITIALIZER;
hook_malloc		__malloc_hook = &lib_malloc_hook;
pthread_mutex_t		g_malloc_mutex = PTHREAD_MUTEX_INITIALIZER;

static void		mem_is_separated(void *sbrk_ret,
					 size_t x,
					 int page_size)
{
  ((t_bloc_meta_data *)(sbrk_ret))->next = 0;
  ((t_bloc_meta_data *)(sbrk_ret))->prev = g_last_bloc;
  ((t_bloc_meta_data *)(sbrk_ret))->size = (x * page_size) - META_DATA_SIZE;
  ((t_bloc_meta_data *)(sbrk_ret))->next_free = 0;
  ((t_bloc_meta_data *)(sbrk_ret))->prev_free = g_last_bloc;
  ((t_bloc_meta_data *)(sbrk_ret))->is_free = TRUE;
  if (g_last_bloc)
    {
      g_last_bloc->next = sbrk_ret;
      g_last_bloc->next_free = sbrk_ret;
      g_last_bloc = sbrk_ret;
    }
  else
    {
      g_last_bloc = sbrk_ret;
      g_first_bloc = sbrk_ret;
      g_first_free = sbrk_ret;
    }
}

static enum e_status	add_page(size_t asked)
{
  size_t		x;
  void			*sbrk_ret;
  static int		page_size = 0;

  if (page_size <= 0 && (page_size = getpagesize()) <= 0)
    return (FAILURE);
  if (g_last_bloc && g_last_bloc->is_free == TRUE &&
      sbrk(0) == (void *)g_last_bloc + META_DATA_SIZE
      + g_last_bloc->size)
    x = asked / page_size + 1;
  else
    x = ((asked + META_DATA_SIZE) / page_size) + 1;
  if ((((sbrk_ret = sbrk(x * page_size))) == (void *) -1))
    return (FAILURE);
  if (g_last_bloc == 0 && g_first_bloc == 0)
    mem_is_separated(sbrk_ret, x, page_size);
  else if (sbrk_ret != (void *)g_last_bloc + META_DATA_SIZE +
	   g_last_bloc->size && g_last_bloc->is_free == TRUE)
    mem_is_separated(sbrk_ret, x, page_size);
  else if (g_last_bloc->is_free == TRUE)
    g_last_bloc->size += (x * page_size);
  else
    mem_is_separated(sbrk_ret, x, page_size);
  return (SUCCESS);
}

static void		*unlock_mutex_and_return_null()
{
  pthread_mutex_unlock(&g_free_mutex);
  return (0);
}

static void		*follow_malloc(size_t size,
				       t_bloc_meta_data *tmp)
{
  while (tmp)
    {
      if (tmp->size == size && tmp != g_last_bloc)
	{
	  tmp->is_free = FALSE;
	  if (tmp->prev_free)
	    tmp->prev_free->next_free = tmp->next_free;
	  else
	    {
	      g_first_free = tmp->next_free;
	      if (g_first_free)
	      g_first_free->prev_free = 0;
	    }
	  if (tmp->next_free)
	    tmp->next_free->prev_free = tmp->prev_free;
	  pthread_mutex_unlock(&g_free_mutex);
	  return ((void *)tmp + META_DATA_SIZE);
	}
      if (tmp->size > size + META_DATA_SIZE + BLOC_MIN_SIZE)
	return (split(tmp, size));
      tmp = tmp->next_free;
    }
  if (add_page(size) == FAILURE)
    return (unlock_mutex_and_return_null());
  return (split(g_last_bloc, size));
}

void			*malloc(size_t size)
{
  pthread_mutex_lock(&g_malloc_mutex);
  if (__malloc_hook)
    return (__malloc_hook(size, &malloc));
  else
    pthread_mutex_unlock(&g_malloc_mutex);
  if (size == 0 || size > DOUBLE_MAX)
    return (0);
  size = (((size - 1) >> 3) << 3) + 8;
  if (size > DOUBLE_MAX)
    return (0);
  pthread_mutex_lock(&g_free_mutex);
  if (g_first_bloc == 0 &&
      add_page(size) == FAILURE)
    return (unlock_mutex_and_return_null());
  return (follow_malloc(size, g_first_free));
}
