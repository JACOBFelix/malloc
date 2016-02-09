#include <string.h>
#include "malloc.h"

void			*realloc(void *ptr, size_t size)
{
  t_bloc_meta_data	*new;
  t_bloc_meta_data	*tmp;

  if (ptr == 0)
    return (malloc(size));
  if ((new = malloc(size)) == 0)
    return (0);
  tmp = ptr - sizeof(t_bloc_meta_data);
  if (tmp->size > size)
    memcpy(new, ptr, size);
  else
    memcpy(new, ptr, tmp->size);
  free(ptr);
  return (new);
}


/* static void		*split(void *old, size_t asked) */
/* { */
/*   void			*new_bloc; */
/*   void			*old_next; */
/*   t_bloc_meta_data	*tmp; */

/*   old_next = ((t_bloc_meta_data *)(old))->next; */
/*   new_bloc = old + sizeof(t_bloc_meta_data) + asked; */
/*   ((t_bloc_meta_data *)(new_bloc))->is_free = TRUE; */
/*   ((t_bloc_meta_data *)(new_bloc))->size = ((t_bloc_meta_data *)(old))->size */
/*     - asked - META_DATA_SIZE; */
/*   ((t_bloc_meta_data *)(new_bloc))->next = ((t_bloc_meta_data *)(old))->next; */
/*   ((t_bloc_meta_data *)(old))->next = new_bloc; */
/*   ((t_bloc_meta_data *)(old))->is_free = FALSE; */
/*   ((t_bloc_meta_data *)(old))->size = asked; */
/*   ((t_bloc_meta_data *)(new_bloc))->prev = old; */
/*   ((t_bloc_meta_data *)(new_bloc))->next_free = ((t_bloc_meta_data *)(old))->next_free; */
/*   ((t_bloc_meta_data *)(new_bloc))->prev_free = ((t_bloc_meta_data *)(old))->prev_free; */
/*   if (((tmp = ((t_bloc_meta_data *)(old))->next_free)) != 0) */
/*   tmp->prev_free = new_bloc; */
/*   if (((tmp = ((t_bloc_meta_data *)(old))->prev_free)) != 0) */
/*     tmp->next_free = new_bloc; */
/*   if (g_first_free == old) */
/*     g_first_free = new_bloc; */

/*    if (g_last_bloc == old) */
/*      g_last_bloc = ((t_bloc_meta_data *)(new_bloc)); */
/*   else */
/*     ((t_bloc_meta_data *)(old_next))->prev = new_bloc; */
/*   return (old + META_DATA_SIZE); */
/* } */

/* static void			cat_bloc(t_bloc_meta_data *first, */
/* 					 t_bloc_meta_data *sec) */
/* { */
/*   t_bloc_meta_data		*third; */
/*   t_bloc_meta_data		*tmp; */

/*   third = sec->next; */
/*   first->size += sec->size + META_DATA_SIZE; */
/*   first->next = third; */
/*   if (third == 0) */
/*     g_last_bloc = first; */
/*   else */
/*     third->prev = first; */
/*   first->next_free = sec->next_free; */
/*   if ((tmp = sec->next_free)) */
/*     tmp->prev_free = first; */
/* } */

/* static enum e_status	is_inlist(t_bloc_meta_data *ptr) */
/* { */
/*   t_bloc_meta_data	*tmp; */

/*   tmp = g_first_bloc; */
/*   while (tmp) */
/*     { */
/*       if (tmp == ptr) */
/* 	return (SUCCESS); */
/*       tmp = tmp->next; */
/*     } */
/*   return (FAILURE); */
/* } */

/* void			*realloc(void *ptr, size_t size) */
/* { */
/*   t_bloc_meta_data	*tmp; */
/*   void			*ptr2; */
/*   t_bloc_meta_data	*s_tmp; */

/*   if (ptr == 0) */
/*     return (malloc(size)); */
/*   tmp = ptr - META_DATA_SIZE; */
/*   if (is_inlist(tmp) == FAILURE) */
/*     return (0); */
/*   if (size == 0) */
/*     { */
/*       free(tmp); */
/*       return (0); */
/*     } */
/*   size = (((size - 1) / 16) * 16 ) + 16; */
/*   if (size == tmp->size) */
/*     return (ptr); */
/*   if (tmp->next && tmp->next->is_free == TRUE */
/*       && size + META_DATA_SIZE + BLOC_MIN_SIZE <= */
/* 	  tmp->size + tmp->next->size + META_DATA_SIZE) */
/*     { */
/*       if (((s_tmp = tmp->next) != 0) && ((s_tmp->prev_free) != 0)) */
/* 	{ */
/* 	  s_tmp->prev_free->next_free = tmp; */
/* 	  tmp->prev_free = s_tmp->prev_free; */
/* 	} */
/*       /\* tmp->next_free = tmp->next->next_free; *\/ */
/*       /\* tmp->prev_free = tmp->next->prev_free; *\/ */
/*       cat_bloc(tmp, tmp->next); */
/*       return (split(tmp, size) + META_DATA_SIZE); */
/*     } */
/*   else */
/*     { */
/*       if ((ptr2 = malloc(size)) == 0) */
/* 	return (0); */
/*       memcpy(ptr2, ptr, size); */
/*       free(ptr - META_DATA_SIZE); */
/*       return (ptr2); */
/*     } */
/* } */
