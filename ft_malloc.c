
// verify all the functions return

#include "ft_malloc.h"

pthread_mutex_t	malloc_mutex = PTHREAD_MUTEX_INITIALIZER;
t_heap			*g_heap = NULL;


void	free(void *ptr);
void	*malloc(size_t size);
void	*calloc(size_t count, size_t size);
void	*realloc(void *ptr, size_t size);
void	show_alloc_mem(void);
void	show_alloc_mem_ex(void);
void	defragment_mem(void);

int	init_heap(void)
{
	t_heap	*heap;

	heap = allocate_mem(sizeof(t_heap));

	if (!heap)
		return 1;
	heap->tiny = NULL;
	heap->small = NULL;
	heap->large = NULL;
	g_heap = heap;
	return 0;
}


void	*allocate_mem(size_t size)
{
	void	*ptr;

	// Round up to page size multiple
	size_t aligned_size = (size + getpagesize() - 1) & ~(getpagesize() - 1);
	ptr = mmap(NULL, aligned_size,
			PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (ptr == MAP_FAILED)
		return NULL;

	return ptr;
}

void	*malloc(size_t size)
{
	t_block	*block;

	// try system limit size
	// add mutex
	if (!g_heap)
		init_heap();
	if (size <= SMALL_SIZE)
		block = get_free_block(size);
	else
		block = create_large_zone(size + sizeof(t_block) + sizeof(t_zone));
	block->free = 0;
	return (void *)((char *)block + sizeof(t_block));
}
