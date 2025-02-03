
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
		block = create_large_zone(size + BLOCK_SIZE + sizeof(t_zone));
	block->free = 0;
	return (void *)((char *)block + BLOCK_SIZE);
}

void	*calloc(size_t count, size_t size)
{
	size_t	tot_size;
	char	*ptr;

	tot_size = count * size;
	ptr = malloc(tot_size);
	while (--tot_size >= 0)
		*(ptr + tot_size) = 0;

	return ptr;
}

void	free(void *ptr)
{
	t_block	*curr_block;
	t_zone	*curr_zone;
	t_zone	*head_zone = NULL;

	curr_block = (char *)ptr - BLOCK_SIZE;
	curr_block->free = 1;

	if (curr_block->size > SMALL_SIZE)
	{
		if (g_heap->large->blocks->free)
			g_heap->large = g_heap->large->next;
		else
			head_zone = g_heap->large;

		while (head_zone)
		{
			if (head_zone->next->blocks->free)
			{
				head_zone->next = head_zone->next->next;
				break;
			}
			head_zone = head_zone->next;
		}
	// if (curr_block->size > SMALL_SIZE)
	// {
	// 	head_zone = g_heap->large;
	// 	curr_zone = curr_block - sizeof(t_zone);
	// 	if (head_zone == curr_zone)
	// 		g_heap->large = g_heap->large->next;
	// 	while (head_zone && head_zone != curr_zone)
	// 	{
	// 		if (head_zone->next == curr_zone)
	// 		{
	// 			head_zone->next = head_zone->next->next;
	// 			break;
	// 		}
	// 		head_zone = head_zone->next;
	// 	}
		munmap(curr_zone, curr_block->size + BLOCK_SIZE + sizeof(t_zone)); //pagesize()?
	}
	else
	{
		zone_collapse();
	}
	return;
}

void	*realloc(void *ptr, size_t new_size)
{
	t_block	*curr_block;
	char	*new_ptr;
	char	*curr_ptr;
	size_t	old_size;

	curr_ptr = (char *)ptr;
	curr_block = curr_ptr - BLOCK_SIZE;
	old_size = curr_block->size;
	if (curr_block->size == new_size)
		return ptr;
	new_ptr = block_expand(curr_block, new_size);
	if (!new_ptr)
	{
		new_ptr = malloc(new_size);
		for (size_t i = 0; i < old_size; i++) // update to add security
			new_ptr[i] = curr_ptr[i];

		free(ptr);
	}

	return new_ptr;
}
