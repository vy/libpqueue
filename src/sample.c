#include <stdio.h>
#include <stdlib.h>

#include "pqueue.h"


typedef struct node_t
{
	long   pri;
	int    val;
	size_t pos;
} node_t;


static long
get_pri(void *a)
{
	return ((node_t *) a)->pri;
}


static void
set_pri(void *a, long pri)
{
	((node_t *) a)->pri = pri;
}


static size_t
get_pos(void *a)
{
	return ((node_t *) a)->pos;
}


static void
set_pos(void *a, size_t pos)
{
	((node_t *) a)->pos = pos;
}
	

int
main(void)
{
	pqueue_t *pq;
	node_t   *ns;
	node_t   *n;

	ns = malloc(10 * sizeof(node_t));
	pq = pq_init(10, get_pri, set_pri, get_pos, set_pos);
	if (!(ns && pq)) return 1;

	ns[0].pri = 5; ns[0].val = -5; pq_insert(pq, &ns[0]);
	ns[1].pri = 4; ns[1].val = -4; pq_insert(pq, &ns[1]);
	ns[2].pri = 2; ns[2].val = -2; pq_insert(pq, &ns[2]);
	ns[3].pri = 6; ns[3].val = -6; pq_insert(pq, &ns[3]);
	ns[4].pri = 1; ns[4].val = -1; pq_insert(pq, &ns[4]);

	n = pq_peek(pq);
	printf("peek: %ld @ %ul\n", n->pri, n->pos);

	pq_change_priority(pq, 8, &ns[4]);
	pq_change_priority(pq, 7, &ns[2]);

   	while ((n = pq_pop(pq)))
		printf("pop: %ld @ %ul\n", n->pri, n->pos);

	pq_free(pq);
	free(ns);

	return 0;
}
