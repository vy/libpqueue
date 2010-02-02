/*
 * Copyright 2010 Volkan Yazıcı <volkan.yazici@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */


/**
 * There are two ways to store multi-attribute priorities in the heap.
 *
 * 1) Modify pqueue.h and replace "pqueue_pri_t" with a "void *", instead of
 *    "double". Now in your "node_t" struct, you can represent your node
 *    priority in any way you like: Using an "int" array, "struct", etc.
 *
 *    In this scheme, you will also need to pass fresh memory blocks to
 *    "pqueue_change_priority" function calls and free previous priority data
 *    structure manually. (A wrapper over "pqueue_change_priority" can handle
 *    this easily. Also using a GC (e.g. Boehm GC) would ease the work and
 *    improve performance -- consider thousands of tiny allocations.)
 *
 *    Pros:
 *    - Clean, flexible design. (Open to further improvements.)
 *    - Ability to use any operator to compare any desired priority data type.
 *
 *    Cons:
 *    - Requires modification in "pqueue.h" and a from scratch compilation of
 *      "pqueue.c".
 *
 * 2) Use a seperate vector to store the priorities and make "priority" slot of
 *    "node_t" structures to point to the cells of this vector.
 *
 *    Pros:
 *    - No outside modification is required. (Vanilla libpqueue is ok.)
 *    - It works.
 *
 *    Cons:
 *    - A nasty, error-prone design.
 *
 * This file represents a sample implementation for the second scheme.
 */


#include <stdio.h>
#include <stdlib.h>

#include "pqueue.h"


static int **pris;


typedef struct node_t
{
	int    pri;
	int    val;
	size_t pos;
} node_t;


static int
cmp_pri(double next, double curr)
{
	int *_next = pris[(int) next];
	int *_curr = pris[(int) curr];

	return
		(_next[0] >  _curr[0]) ||
		(_next[0] == _curr[0]  && _next[1] > _curr[1]);
}


static double
get_pri(void *a)
{
	return (double) ((node_t *) a)->pri;
}


static void
set_pri(void *a, double pri)
{
	((node_t *) a)->pri = (int) pri;
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


static void
pr_node(FILE *out, void *a)
{
	node_t *n = a;

	fprintf(out, "pri: %d, val: %d, real-val: [%d %d]\n",
			n->pri, n->val, pris[n->pri][0], pris[n->pri][1]);
}


int
main(void)
{
	int i;
	int p;

	pqueue_t *pq;
	node_t   *ns;
	node_t   *n;

	/* We will need (N + 1) slots in "pris" vector. Extra one slot for spare
	 * usages. */
	pris = malloc(5 * sizeof(int *));
	for (i = 0; i < 5; i++)
		pris[i] = malloc(2 * sizeof(int));

	pris[0][0] = 4; pris[0][1] = 2;
	pris[1][0] = 3; pris[1][1] = 7;
	pris[2][0] = 3; pris[2][1] = 1;
	pris[3][0] = 5; pris[3][1] = 6;
	p = 4;									/* Initialize spare slot. */
	
	pq = pqueue_init(10, cmp_pri, get_pri, set_pri, get_pos, set_pos);
	ns = malloc(4 * sizeof(node_t));
	
	ns[0].pri = 0; ns[0].val = 0; pqueue_insert(pq, &ns[0]);
	ns[1].pri = 1; ns[0].val = 1; pqueue_insert(pq, &ns[1]);
	ns[2].pri = 2; ns[0].val = 2; pqueue_insert(pq, &ns[2]);
	ns[3].pri = 3; ns[0].val = 3; pqueue_insert(pq, &ns[3]);

	printf("initial:\n"); pqueue_print(pq, stdout, pr_node);

	n = pqueue_pop(pq);
	printf("[pop] pri: %d, val: %d, real-pri: [%d %d]\n",
		   n->pri, n->val, pris[n->pri][0], pris[n->pri][1]);
	printf("after first pop:\n"); pqueue_print(pq, stdout, pr_node);

	pris[p][0] = 3; pris[p][1] = 0;
	pqueue_change_priority(pq, p, &ns[3]);	/* 3: (5,6) -> (3,0) */
	p = 3;									/* Move spare slot to 3. */
	printf("after 3: (5,6) -> (3,0):\n"); pqueue_print(pq, stdout, pr_node);

	pris[p][0] = 3; pris[p][1] = -1;
	pqueue_change_priority(pq, p, &ns[0]);	/* 0: (4,2) -> (3,-1) */
	p = 0;									/* Move spare slot to 0. */
	printf("after 0: (4,2) -> (3,-1):\n"); pqueue_print(pq, stdout, pr_node);

	while ((n = pqueue_pop(pq)))
		printf("[pop] pri: %d, val: %d, real-pri: [%d %d]\n",
			   n->pri, n->val, pris[n->pri][0], pris[n->pri][1]);

	pqueue_free(pq);
	free(ns);
	free(pris);

	return 0;
}

/*
 * $ cc -Wall -g pqueue.c sample-multiattr.c -o sample-multiattr
 * $ ./sample-multiattr
 * initial:
 * pri: 2, val: 0, real-val: [3 1]
 * pri: 1, val: 0, real-val: [3 7]
 * pri: 0, val: 3, real-val: [4 2]
 * pri: 3, val: 0, real-val: [5 6]
 * [pop] pri: 2, val: 0, real-pri: [3 1]
 * after first pop:
 * pri: 1, val: 0, real-val: [3 7]
 * pri: 0, val: 3, real-val: [4 2]
 * pri: 3, val: 0, real-val: [5 6]
 * after 3: (5,6) -> (3,0):
 * pri: 4, val: 0, real-val: [3 0]
 * pri: 1, val: 0, real-val: [3 7]
 * pri: 0, val: 3, real-val: [4 2]
 * after 0: (4,2) -> (3,-1):
 * pri: 3, val: 3, real-val: [3 -1]
 * pri: 4, val: 0, real-val: [3 0]
 * pri: 1, val: 0, real-val: [3 7]
 * [pop] pri: 3, val: 3, real-pri: [3 -1]
 * [pop] pri: 4, val: 0, real-pri: [3 0]
 * [pop] pri: 1, val: 0, real-pri: [3 7]
 */
