#include "malloc.h"

extern pthread_mutex_t		g_malloc_mutex;
extern pthread_mutex_t		g_mutex_free_mutex;

void				*lib_malloc_hook(size_t size, const void *caller)
{
  hook_malloc			tmp;
  void				*ret;
  static pthread_mutex_t	hook_mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&hook_mutex);
  tmp = __malloc_hook;
  __malloc_hook = 0;
  pthread_mutex_unlock(&g_malloc_mutex);
  ret = malloc(size);
  __malloc_hook = tmp;
  pthread_mutex_unlock(&hook_mutex);
  return (ret);
  (void)caller;
}

void				lib_free_hook(void *ptr,
					      const void *caller)
{
  hook_free			tmp;
  static pthread_mutex_t	hook_mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&hook_mutex);
  tmp = __free_hook;
  __free_hook = 0;
  pthread_mutex_unlock(&g_mutex_free_mutex);
  free(ptr);
  __free_hook = tmp;
  pthread_mutex_unlock(&hook_mutex);
  return ;
  (void)caller;
}
