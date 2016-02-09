#ifndef MALLOC_H_
 #define MALLOC_H_

#include <stdio.h>
#include <pthread.h>

typedef void	*(*hook_malloc)(size_t, const void *);
typedef void	*(*hook_realloc)(void *, size_t, const void *);
typedef void	(*hook_memalign)(size_t, size_t, const void *);
typedef void	(*hook_free)(void *, const void *);
/* typedef void	(*hook_initialize_hook)(void); */
typedef void	(*hook_after_morecore)(void);

enum	e_status
  {
    SUCCESS,
    FAILURE,
  };

enum	e_mutex_key
  {
    TO_UNLOCK,
    TO_NOTHING,
  };

enum	is_free
  {
    TRUE,
    FALSE,
  };

typedef struct				s_bloc_meta_data
{
  enum is_free				is_free;
  size_t				size;
  struct s_bloc_meta_data      		*next;
  struct s_bloc_meta_data	       	*prev;
  struct s_bloc_meta_data		*next_free;
  struct s_bloc_meta_data		*prev_free;
}					t_bloc_meta_data;

extern hook_malloc			__malloc_hook;
extern hook_realloc			__realloc_hook;
/* extern hook_memalign			__memalign_hook; */
extern hook_free			__free_hook;
/* extern hook_initalize_hook		__malloc_initalize_hook; */
/* extern hook_after_morecore		__after_morecore_hook; */
#  define DOUBLE_MAX			9223372036854775807

#define PAGE_SIZE			4096
#define BLOC_MIN_SIZE			8
#define META_DATA_SIZE			sizeof(t_bloc_meta_data)

void			*malloc(size_t);
void			*lib_malloc_hook(size_t size, const void *caller);
void			*calloc(size_t, size_t);
void			show_alloc_mem();
void			free(void *);
void			lib_free_hook(void *ptr, const void *caller);
void			*realloc(void *, size_t);
void			*lib_realloc_hook(void *ptr, size_t size, const void *caller);
int			malloc_trim(size_t pad);
size_t			malloc_usable_size(void *ptr);
enum e_status		is_inlist(t_bloc_meta_data *);
void			*split(void *, size_t);
void			cat_bloc(t_bloc_meta_data *, t_bloc_meta_data *);
void			*my_sbrk_add(size_t x, int page_size);
void			*my_sbrk_sub(size_t size);

#endif /* !MALLOC_H_ */
