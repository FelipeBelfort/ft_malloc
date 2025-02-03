// The library must be named libft_malloc_$HOSTTYPE.so.

// • Your Makefile will have to check the existence of the environment variable $HOSTTYPE. If it is empty or non-existant, to assign the following value:
// ‘uname -m‘_‘uname -s‘
// ifeq ($(HOSTTYPE),)
// HOSTTYPE := $(shell uname -m)_$(shell uname -s)
// endif
// • Your Makefile will have to create a symbolic link libft_malloc.so pointing to
// libft_malloc_$HOSTTYPE.so so for example:
// libft_malloc.so -> libft_malloc_intel-mac.so

// You are allowed a global variable to manage your allocations and one for the threadsafe.
// • Your project must be clean code even without norm if it’s ugly you will get 0

// •Manage the use of your malloc in a multi-threaded program (so to be “thread safe”
// using the pthread lib).
// In order to get the maximum score you have to implement some additional functions
// (non exhaustive list) such as:
// •Manage the malloc debug environment variables. You can imitate those from malloc
// system or invent your own.
// •Create a show_alloc_mem_ex() function that displays more details, for example,
// a history of allocations, or an hexa dump of the allocated zones.
// •“Defragment” the freed memory.

// encapsulate the other functions

#ifndef FT_MALLOC_H
# define FT_MALLOC_H

# include <sys/mman.h>
# include <pthread.h>
# include <unistd.h>

# define TINY_SIZE 128
# define SMALL_SIZE 1024
# define TINY_ZONE_SIZE ((TINY_SIZE * 100) + sizeof(t_block) + sizeof(t_zone))
# define SMALL_ZONE_SIZE ((SMALL_SIZE * 100) + sizeof(t_block) + sizeof(t_zone))
# define BLOCK_SIZE sizeof(t_block)


typedef struct s_block {
	char			free; //bool?
	size_t			size;
	struct s_block	*next;
}	t_block;

typedef struct s_zone {
	// size?
	t_block			*blocks;
	struct s_zone	*next;
}	t_zone;

typedef struct s_heap {
	pthread_mutex_t	mutex;
	t_zone			*tiny;
	t_zone			*small;
	t_zone			*large;
}	t_heap;

extern t_heap	*g_heap;
extern pthread_mutex_t	malloc_mutex;


void	free(void *ptr);
void	*malloc(size_t size);
void	*calloc(size_t count, size_t size);
void	*realloc(void *ptr, size_t size);
void	show_alloc_mem(void);
void	show_alloc_mem_ex(void);
void	defragment_mem(void);

#endif
