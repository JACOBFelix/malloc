#include <stdio.h>
#include <unistd.h>
#include "malloc.h"

extern t_bloc_meta_data	*g_first_bloc;

void			show_alloc_mem()
{
  t_bloc_meta_data	*tmp;

  tmp = g_first_bloc;
  printf("break : %p\n", sbrk(0));
  while (tmp)
    {
      if (tmp->is_free == FALSE)
	{
	      printf("%p - %p : %zu octect(s)\n", tmp, (void *)tmp
		     + sizeof(t_bloc_meta_data) + tmp->size, tmp->size);
	}
      tmp = tmp->next;
    }
}
