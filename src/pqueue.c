/*
 * Copyright 2010 Volkan Yazıcı <volkan.yazici@gmail.com>
 * Copyright 2006-2010 The Apache Software Foundation
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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pqueue.h"


#define left(i)   ((i) << 1)
#define right(i)  (((i) << 1) + 1)
#define parent(i) ((i) >> 1)


pqueue_t *
pq_init(size_t n,
		pqueue_get_priority getpri,
		pqueue_set_priority setpri,
		pqueue_get_position getpos,
		pqueue_set_position setpos)
{
    pqueue_t *q;

    if (!(q = malloc(sizeof(pqueue_t))))
        return NULL;

    /* Need to allocate n+1 elements since element 0 isn't used. */
    if (!(q->d = malloc((n + 1) * sizeof(void *)))) {
        free(q);
        return NULL;
    }
	
    q->size = 1;
    q->avail = q->step = (n+1);  /* see comment above about n+1 */
    q->setpri = setpri;
    q->getpri = getpri;
    q->getpos = getpos;
    q->setpos = setpos;
	
    return q;
}


void
pq_free(pqueue_t *q)
{
    free(q->d);
    free(q);
}


size_t
pq_size(pqueue_t *q)
{
    /* queue element 0 exists but doesn't count since it isn't used. */
    return (q->size - 1);
}


static void
pq_bubble_up(pqueue_t *q, size_t i)
{
    size_t parent_node;
    void *moving_node = q->d[i];
    long moving_pri = q->getpri(moving_node);

    for (parent_node = parent(i);
         ((i > 1) && (q->getpri(q->d[parent_node]) < moving_pri));
         i = parent_node, parent_node = parent(i))
    {
        q->d[i] = q->d[parent_node];
        q->setpos(q->d[i], i);
    }

    q->d[i] = moving_node;
    q->setpos(moving_node, i);
}


static size_t
maxchild(pqueue_t *q, size_t i)
{
    size_t child_node = left(i);

    if (child_node >= q->size)
        return 0;

    if ((child_node+1 < q->size) &&
        (q->getpri(q->d[child_node+1]) > q->getpri(q->d[child_node])))
        child_node++; /* use right child instead of left */

    return child_node;
}


static void
pq_percolate_down(pqueue_t *q, size_t i)
{
    size_t child_node;
    void *moving_node = q->d[i];
    long moving_pri = q->getpri(moving_node);

    while ((child_node = maxchild(q, i)) &&
           (moving_pri < q->getpri(q->d[child_node])))
    {
        q->d[i] = q->d[child_node];
        q->setpos(q->d[i], i);
        i = child_node;
    }

    q->d[i] = moving_node;
    q->setpos(moving_node, i);
}


int
pq_insert(pqueue_t *q, void *d)
{
    void *tmp;
    size_t i;
    size_t newsize;

    if (!q) return 1;

    /* allocate more memory if necessary */
    if (q->size >= q->avail) {
        newsize = q->size + q->step;
        if (!(tmp = realloc(q->d, sizeof(void *) * newsize)))
            return 1;
        q->d = tmp;
        q->avail = newsize;
    }

    /* insert item */
    i = q->size++;
    q->d[i] = d;
    pq_bubble_up(q, i);
	
    return 0;
}


void
pq_change_priority(pqueue_t *q,
				   long new_priority,
				   void *d)
{
    size_t posn;
	long old_priority = q->getpri(d);

	q->setpri(d, new_priority);
    posn = q->getpos(d);
    if (new_priority > old_priority)
        pq_bubble_up(q, posn);
    else
        pq_percolate_down(q, posn);
}


int
pq_remove(pqueue_t *q, void *d)
{
    size_t posn = q->getpos(d);
    q->d[posn] = q->d[--q->size];
    if (q->getpri(q->d[posn]) > q->getpri(d))
        pq_bubble_up(q, posn);
    else
        pq_percolate_down(q, posn);

    return 0;
}


void *
pq_pop(pqueue_t *q)
{
    void *head;

    if (!q || q->size == 1)
        return NULL;

    head = q->d[1];
    q->d[1] = q->d[--q->size];
    pq_percolate_down(q, 1);

    return head;
}


void *
pq_peek(pqueue_t *q)
{
    void *d;
    if (!q || q->size == 1)
        return NULL;
    d = q->d[1];
    return d;
}


void
pq_dump(pqueue_t *q,
		FILE *out,
		pqueue_print_entry print)
{
    int i;

    fprintf(stdout,"posn\tleft\tright\tparent\tmaxchild\t...\n");
    for (i = 1; i < q->size ;i++) {
        fprintf(stdout,
                "%d\t%d\t%d\t%d\t%ul\t",
                i,
                left(i), right(i), parent(i),
                maxchild(q, i));
        print(out, q->d[i]);
    }
}


static void
pq_set_pos(void *d, size_t val)
{
    /* do nothing */
}


static void
pq_set_pri(void *d, long pri)
{
	/* do nothing */
}


void
pq_print(pqueue_t *q,
		 FILE *out,
		 pqueue_print_entry print)
{
    pqueue_t *dup;
	
    dup = pq_init(q->size, q->getpri, pq_set_pri, q->getpos, pq_set_pos);
    dup->size = q->size;
    dup->avail = q->avail;
    dup->step = q->step;

    memcpy(dup->d, q->d, (q->size * sizeof(void *)));

    while (pq_size(dup) > 0) {
        void *e = NULL;
        if ((e = pq_pop(dup)))
            print(out, e);
        else
            break;
    }
	
    pq_free(dup);
}


static int
pq_subtree_is_valid(pqueue_t *q, int pos)
{
    if (left(pos) < q->size) {
        /* has a left child */
        if (q->getpri(q->d[pos]) < q->getpri(q->d[left(pos)]))
            return 0;
        if (!pq_subtree_is_valid(q, left(pos)))
            return 0;
    }
    if (right(pos) < q->size) {
        /* has a right child */
        if (q->getpri(q->d[pos]) < q->getpri(q->d[right(pos)]))
            return 0;
        if (!pq_subtree_is_valid(q, right(pos)))
            return 0;
    }
    return 1;
}


int
pq_is_valid(pqueue_t *q)
{
    return pq_subtree_is_valid(q, 1);
}
