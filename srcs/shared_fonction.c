#include "malloc.h"

extern t_bloc_meta_data	*g_first_bloc;
extern t_bloc_meta_data	*g_last_bloc;
extern t_bloc_meta_data	*g_first_free;
extern pthread_mutex_t	g_free_mutex;

enum e_status	is_inlist(t_bloc_meta_data *ptr)
{
  t_bloc_meta_data	*tmp;

  tmp = g_first_bloc;
  while (tmp)
    {
      if (tmp == ptr)
	return (SUCCESS);
      tmp = tmp->next;
    }
  return (FAILURE);
}

static void		malloc_free_list_handler(t_bloc_meta_data *new_bloc,
						 t_bloc_meta_data *old)
{
  t_bloc_meta_data	*tmp;

  
  ((t_bloc_meta_data *)(new_bloc))->next_free =
    ((t_bloc_meta_data *)(old))->next_free;
  ((t_bloc_meta_data *)(new_bloc))->prev_free =
    ((t_bloc_meta_data *)(old))->prev_free;
  if (((tmp = ((t_bloc_meta_data *)(old))->next_free)) != 0)
    tmp->prev_free = new_bloc;
  if (((tmp = ((t_bloc_meta_data *)(old))->prev_free)) != 0)
    tmp->next_free = new_bloc;
  if (g_first_free == old)
    g_first_free = new_bloc;
  if (new_bloc < g_first_free)
    {
      new_bloc->next_free = g_first_free;
      new_bloc->prev_free = 0;
      g_first_free->prev_free = new_bloc;
      g_first_free = new_bloc;
    }
}

void			cat_bloc(t_bloc_meta_data *first,
				 t_bloc_meta_data *sec)
{
  t_bloc_meta_data	*third;
  t_bloc_meta_data	*tmp;

  if ((void *)first + first->size + META_DATA_SIZE != sec)
    return ;
  third = sec->next;
  first->size += sec->size + META_DATA_SIZE;
  first->next = third;
  if (third == 0)
    g_last_bloc = first;
  else
    third->prev = first;
  first->next_free = sec->next_free;
  if ((tmp = sec->next_free))
    tmp->prev_free = first;
}

void		*split(void *old, size_t asked)
{
  void		*new_bloc;
  void  	*old_next;

  old_next = ((t_bloc_meta_data *)(old))->next;
  new_bloc = old + META_DATA_SIZE + asked;
  ((t_bloc_meta_data *)(new_bloc))->is_free = TRUE;
  ((t_bloc_meta_data *)(new_bloc))->size = ((t_bloc_meta_data *)(old))->size
    - asked - META_DATA_SIZE;
  ((t_bloc_meta_data *)(new_bloc))->next = ((t_bloc_meta_data *)(old))->next;
  ((t_bloc_meta_data *)(old))->next = new_bloc;
  ((t_bloc_meta_data *)(old))->is_free = FALSE;
  ((t_bloc_meta_data *)(old))->size = asked;
  ((t_bloc_meta_data *)(new_bloc))->prev = old;
  malloc_free_list_handler(new_bloc, old);
  if (g_last_bloc == old)
     g_last_bloc = ((t_bloc_meta_data *)(new_bloc));
   else
     ((t_bloc_meta_data *)(old_next))->prev = new_bloc;
  pthread_mutex_unlock(&g_free_mutex);
  return (old + META_DATA_SIZE);
}
