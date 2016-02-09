#include <string.h>
#include "malloc.h"

extern t_bloc_meta_data	*g_first_bloc;
extern t_bloc_meta_data	*g_last_bloc;
extern t_bloc_meta_data	*g_first_free;
extern pthread_mutex_t	g_free_mutex;
hook_realloc		__realloc_hook = lib_realloc_hook;
pthread_mutex_t		g_realloc_mutex = PTHREAD_MUTEX_INITIALIZER;

void				*lib_realloc_hook(void *ptr,
						  size_t size,
						  const void *caller)
{
  hook_realloc			tmp;
  static pthread_mutex_t	hook_mutex = PTHREAD_MUTEX_INITIALIZER;
  void				*ret;

  pthread_mutex_lock(&hook_mutex);
  tmp = __realloc_hook;
  __realloc_hook = 0;
  pthread_mutex_unlock(&g_realloc_mutex);
  ret = realloc(ptr, size);
  __realloc_hook = tmp;
  pthread_mutex_unlock(&hook_mutex);
  return (ret);
  (void)caller;
}

static void		*realloc_call_malloc(void *ptr,
					     size_t size,
					     t_bloc_meta_data *md_ptr)
{
  void			*tmp;

  if ((tmp = malloc(size)) == 0)
    {
      free(ptr);
      return (0);
    }
  memcpy(tmp, ptr, md_ptr->size);
  free(ptr);
  return (tmp);
}

static void		*follow_realloc(void *ptr,
					size_t size,
					t_bloc_meta_data *md_ptr)
{
  if (size == md_ptr->size)
    return (ptr);

  if (size + META_DATA_SIZE + BLOC_MIN_SIZE <= md_ptr->size)
    {
      pthread_mutex_lock(&g_free_mutex);
      return (split(md_ptr, size));
    }
  return (realloc_call_malloc(ptr, size, md_ptr));
}

void			*realloc(void *ptr, size_t size)
{
  /* pthread_mutex_lock(&g_realloc_mutex); */
  /* if (__realloc_hook) */
  /*   return (__realloc_hook(ptr, size, &realloc)); */
  /* else */
  /*   pthread_mutex_unlock(&g_realloc_mutex); */
  if (ptr == 0)
    return (malloc(size));
  if (size == 0 || size > DOUBLE_MAX)
    {
      free(ptr);
      return (0);
    }
  if (is_inlist(ptr - META_DATA_SIZE) == FAILURE)
    {
      free(ptr);
      return (0);
    }
  size = (((size - 1) >> 3 ) << 3) + 8;
  if (size > DOUBLE_MAX)
    {
      free(ptr);
      return (0);
    }
  return (follow_realloc(ptr, size, ptr - META_DATA_SIZE));
}
