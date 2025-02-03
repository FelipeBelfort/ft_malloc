#include "ft_malloc.h"

void	block_collapse(t_zone *target_zone)
{
	t_block	*curr_block;

	if (!target_zone)
		return;
	curr_block = target_zone->blocks;
	while (curr_block && curr_block->next)
	{
		if (curr_block->free && curr_block->next->free)
		{
			curr_block->size += curr_block->next->size + BLOCK_SIZE;
			curr_block->next = curr_block->next->next;
		}
		else
			curr_block = curr_block->next;
	}

}

void	block_split(t_block *block, size_t size)
{
	t_block	*new;

	if (block->size == size)
		return;
	new = (t_block *)((char *)block + BLOCK_SIZE + size + 1);
	new->next = block->next;
	new->free = 1;
	new->size = block->size - size - BLOCK_SIZE;
	block->size = size;
	block->next = new;
}

void	*block_expand(t_block *block, size_t new_size)
{
	size_t	size_to_cmp;

	size_to_cmp = block->size;
	if (block->next && block->next->free)
		size_to_cmp += block->next->size + BLOCK_SIZE;
	if (size_to_cmp >= new_size + BLOCK_SIZE)
	{
		if (size_to_cmp != block->size)
		{
			block->next = block->next->next;
			block->size = size_to_cmp;
		}
		block_split(block, new_size);
		return ((char *)block + BLOCK_SIZE);
	}
	// ajeitar o retorno
	return NULL;
}

t_block	*find_free_block(size_t size, t_zone *ptr)
{
	t_zone	*zone;
	t_block	*block;

	if (!ptr)
		return NULL;

	zone = ptr;
	while (zone)
	{
		block = zone->blocks;
		while (block)
		{
			if (block->free &&
				(block->size == size || block->size >= size + BLOCK_SIZE))
				return block;
			block = block->next;
		}
		zone = zone->next;
	}
	return NULL;

}

t_block	*get_free_block(size_t size)
{
	t_block	*block;
	t_zone	*zone;
	size_t	zone_size;

	zone = g_heap->small;
	zone_size = SMALL_ZONE_SIZE;
	if (size <= TINY_SIZE)
	{
		zone = g_heap->tiny;
		zone_size = TINY_ZONE_SIZE;
	}
	block = find_free_block(size, zone);
	if (!block)
	{
		zone = create_zone(zone_size, &zone);
		block = find_free_block(size, zone);
	}
	block_split(block, size);

	return block;
}


