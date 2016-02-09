#include <unistd.h>
#include "malloc.h"

extern t_bloc_meta_data	*g_first_bloc;
extern t_bloc_meta_data	*g_last_bloc;
extern t_bloc_meta_data	*g_first_free;
extern pthread_mutex_t	g_free_mutex;
hook_free		__free_hook = &lib_free_hook;
pthread_mutex_t		g_mutex_free_mutex = PTHREAD_MUTEX_INITIALIZER;

static void		free_free_list(t_bloc_meta_data *md_ptr,
				       t_bloc_meta_data *tmp)
{
  if (g_first_free == 0)
    {
      g_first_free = md_ptr;
      md_ptr->next_free = 0;
      md_ptr->prev_free = 0;
    }
  else if (md_ptr < g_first_free)
    {
      g_first_free->prev_free = md_ptr;
      md_ptr->next_free = g_first_free;
      md_ptr->prev_free = 0;
      g_first_free = md_ptr;
    }
  else
    {
      tmp = g_first_free;
      while (tmp->next_free != 0 && tmp < md_ptr)
  	tmp = tmp->next_free;
      md_ptr->next_free = tmp;
      if (((tmp = tmp->prev_free)) != 0)
  	tmp->next_free = md_ptr;
      md_ptr->prev_free = tmp;
      if (((tmp = md_ptr->next_free)) != 0)
  	tmp->prev_free = md_ptr;
    }
}

static void		follow_free_check_bloc_free()
{
  t_bloc_meta_data	*tmp;

  if (g_last_bloc->size + META_DATA_SIZE % 4096 == 0)
    {
      tmp = g_last_bloc->prev;
      tmp->next = 0;
      if (g_last_bloc == g_first_free)
	g_first_free = 0;
      else
	g_last_bloc->prev_free->next_free = 0;
      brk(g_last_bloc);
      g_last_bloc = tmp;
    }
  else if (g_last_bloc->size > PAGE_SIZE)
    {
      while (g_last_bloc->size > PAGE_SIZE)
	{
	  if (sbrk(-PAGE_SIZE) == (void *) -1)
	    return ;
	  g_last_bloc->size -= PAGE_SIZE;
	}
    }
}


static void		free_check_bloc_free()
{
  if (g_last_bloc->size + META_DATA_SIZE < PAGE_SIZE ||
      sbrk(0) != (void *)g_last_bloc + META_DATA_SIZE + g_last_bloc->size)
    return ;
  if (g_last_bloc == g_first_bloc)
    {
      if (brk((void *)g_first_bloc) == -1)
	return ;
      g_last_bloc = 0;
      g_first_free = 0;
      g_first_bloc = 0;
      return ;
    }
  follow_free_check_bloc_free();
}

static void		follow_free(t_bloc_meta_data *md_ptr)
{
  t_bloc_meta_data	*tmp;

  /* pthread_mutex_lock(&g_free_mutex); */
  md_ptr->is_free = TRUE;
  free_free_list(md_ptr, 0);
  tmp = md_ptr->next;
  if (g_last_bloc != 0 && tmp != 0 && tmp->is_free == TRUE &&
      ((void *)md_ptr + META_DATA_SIZE + md_ptr->size == (void *)tmp))
    cat_bloc(md_ptr, tmp);
  tmp = md_ptr->prev;
  if (tmp != 0 && tmp->is_free == TRUE &&
      (void *)tmp + tmp->size + META_DATA_SIZE == (void *)md_ptr)
    cat_bloc(tmp, md_ptr);
  if (g_last_bloc && g_last_bloc->is_free == TRUE)
    free_check_bloc_free();
  if (g_last_bloc == g_first_bloc && g_last_bloc &&
      g_last_bloc->is_free == TRUE && sbrk(0) == (void *)g_last_bloc
      + META_DATA_SIZE + g_last_bloc->size && brk(g_last_bloc) == -1)
    {
      g_last_bloc = 0;
      g_first_bloc = 0;
      g_first_free = 0;
    }
  /* pthread_mutex_unlock(&g_free_mutex); */
}

void			free(void *ptr)
{
  t_bloc_meta_data	*md_ptr;

  pthread_mutex_lock(&g_mutex_free_mutex);
  if (__free_hook)
    {
      __free_hook(ptr, &free);
      return ;
    }
  else
    pthread_mutex_unlock(&g_mutex_free_mutex);
  pthread_mutex_lock(&g_free_mutex);
  if (ptr == 0 || ptr < (void *)g_first_bloc ||
      ptr > (void *)g_last_bloc + META_DATA_SIZE)
    {
      pthread_mutex_unlock(&g_free_mutex);
      return ;
    }
  md_ptr = ptr - META_DATA_SIZE;
  if (is_inlist(md_ptr) == FAILURE || md_ptr->is_free != FALSE)
    {
      pthread_mutex_unlock(&g_free_mutex);
      return ;
    }
  follow_free(md_ptr);
  pthread_mutex_unlock(&g_free_mutex);
}
