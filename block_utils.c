#include "ft_malloc.h"

void	block_split(t_block *block, size_t size)
{
	t_block	*new;

	if (block->size == size)
		return;
	new = (t_block *)((char *)block + sizeof(t_block) + size + 1);
	new->next = block->next;
	new->free = 1;
	new->size = block->size - size - sizeof(t_block);
	block->size = size;
	block->next = new;
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
				(block->size == size || block->size >= size + sizeof(t_block)))
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


