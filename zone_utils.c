#include "ft_malloc.h"

t_zone	*create_new_zone(size_t size)
{
	t_zone	*zone;

	zone = allocate_mem(size);
	if (!zone)
		return NULL;

	zone->blocks = (t_block *)((char *)zone + sizeof(t_zone));
	zone->blocks->free = 1;
	zone->blocks->size = size - sizeof(t_block) - sizeof(t_zone);
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

void	*create_large_zone(size_t size)
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
