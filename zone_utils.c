#include "ft_malloc.h"

t_zone	*create_new_zone(size_t size)
{
	t_zone	*zone;
	size_t	zone_size;

	zone = allocate_mem(size);
	if (!zone)
		return NULL;

	zone_size = sizeof(t_zone);
	zone->blocks = (t_block *)((char *)zone + zone_size);
	zone->blocks->free = 1;
	zone->blocks->size = size - BLOCK_SIZE - zone_size;
	zone->blocks->next = NULL;
	zone->next = NULL;

	return zone;
}

void	zone_pushback(t_zone **lst, t_zone *zone)
{
	t_zone	*ptr;

	if (!*lst)
		lst = &zone;
	else
	{
		ptr = *lst;
		while (ptr->next)
			ptr = ptr->next;
		ptr->next = zone;
	}
}

void	*create_large_zone(size_t size) //refacto because it's duplicated
{
	t_zone	*ptr;

	ptr = create_new_zone(size);
	zone_pushback(&g_heap->large, ptr);

	return ptr->blocks;
}

t_zone	*create_zone(size_t size, t_zone **lst)
{
	t_zone	*ptr;

	ptr = create_new_zone(size);
	zone_pushback(lst, ptr);

	return ptr;
}

void	zone_collapse_target(t_zone *target_zone)
{
	while (target_zone)
	{
		block_collapse(target_zone);
		target_zone = target_zone->next;
	}
}

void	zone_collapse(void)
{
	zone_collapse_target(g_heap->tiny);
	zone_collapse_target(g_heap->small);
}
