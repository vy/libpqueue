#include <stdio.h>
#include <stdlib.h>

#include "pqueue.h"


typedef struct node_t
{
	double pri;
	int    val;
	size_t pos;
} node_t;


static int
cmp_pri(double next, double curr)
{
	return (next < curr);
}


static double
get_pri(void *a)
{
	return (double) ((node_t *) a)->pri;
}


static void
set_pri(void *a, double pri)
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
	pq = pqueue_init(10, cmp_pri, get_pri, set_pri, get_pos, set_pos);
	if (!(ns && pq)) return 1;

	ns[0].pri = 5; ns[0].val = -5; pqueue_insert(pq, &ns[0]);
	ns[1].pri = 4; ns[1].val = -4; pqueue_insert(pq, &ns[1]);
	ns[2].pri = 2; ns[2].val = -2; pqueue_insert(pq, &ns[2]);
	ns[3].pri = 6; ns[3].val = -6; pqueue_insert(pq, &ns[3]);
	ns[4].pri = 1; ns[4].val = -1; pqueue_insert(pq, &ns[4]);

	n = pqueue_peek(pq);
	printf("peek: %e [%d]\n", n->pri, n->val);

	pqueue_change_priority(pq, 8, &ns[4]);
	pqueue_change_priority(pq, 7, &ns[2]);

   	while ((n = pqueue_pop(pq)))
		printf("pop: %e [%d]\n", n->pri, n->val);

	pqueue_free(pq);
	free(ns);

	return 0;
}

/*
 * $ cc -Wall -g pqueue.c sample.c -o sample
 * $ ./sample
 * peek: 6.000000e+00 [-6]
 * pop: 8.000000e+00 [-1]
 * pop: 7.000000e+00 [-2]
 * pop: 6.000000e+00 [-6]
 * pop: 5.000000e+00 [-5]
 * pop: 4.000000e+00 [-4]
 */
