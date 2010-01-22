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


/**
 * @file  pqueue.h
 * @brief Priority Queue function declarations
 *
 * @{
 */


#ifndef PQUEUE_H
#define PQUEUE_H


/** callback functions to get/set the priority of an element */
typedef long (*pqueue_get_priority)(void *a);
typedef void (*pqueue_set_priority)(void *a, long pri);


/** callback functions to get/set the position of an element */
typedef size_t (*pqueue_get_position)(void *a);
typedef void (*pqueue_set_position)(void *a, size_t pos);


/** debug callback function to print a entry */
typedef void (*pqueue_print_entry)(FILE *out, void *a);


/** the priority queue handle */
typedef struct pqueue_t
{
    size_t size;
    size_t avail;
    size_t step;
    pqueue_get_priority getpri;
    pqueue_set_priority setpri;
    pqueue_get_position getpos;
    pqueue_set_position setpos;
    void **d;
} pqueue_t;


/**
 * initialize the queue
 *
 * @param n the initial estimate of the number of queue items for which memory
 *          should be preallocated
 * @param pri the callback function to run to assign a score to a element
 * @param get the callback function to get the current element's position
 * @param set the callback function to set the current element's position
 *
 * @Return the handle or NULL for insufficent memory
 */
pqueue_t *
pq_init(size_t n,
		pqueue_get_priority getpri,
		pqueue_set_priority setpri,
		pqueue_get_position getpos,
		pqueue_set_position setpos);


/**
 * free all memory used by the queue
 * @param q the queue
 */
void pq_free(pqueue_t *q);


/**
 * return the size of the queue.
 * @param q the queue
 */
size_t pq_size(pqueue_t *q);


/**
 * insert an item into the queue.
 * @param q the queue
 * @param d the item
 * @return 0 on success
 */
int pq_insert(pqueue_t *q, void *d);


/**
 * move an existing entry to a different priority
 * @param q the queue
 * @param old the old priority
 * @param d the entry
 */
void
pq_change_priority(pqueue_t *q,
				   long new_priority,
				   void *d);


/**
 * pop the highest-ranking item from the queue.
 * @param p the queue
 * @param d where to copy the entry to
 * @return NULL on error, otherwise the entry
 */
void *pq_pop(pqueue_t *q);


/**
 * remove an item from the queue.
 * @param p the queue
 * @param d the entry
 * @return 0 on success
 */
int pq_remove(pqueue_t *q, void *d);


/**
 * access highest-ranking item without removing it.
 * @param q the queue
 * @param d the entry
 * @return NULL on error, otherwise the entry
 */
void *pq_peek(pqueue_t *q);


/**
 * print the queue
 * @internal
 * DEBUG function only
 * @param q the queue
 * @param out the output handle
 * @param the callback function to print the entry
 */
void
pq_print(pqueue_t *q, 
		 FILE *out, 
		 pqueue_print_entry print);


/**
 * dump the queue and it's internal structure
 * @internal
 * debug function only
 * @param q the queue
 * @param out the output handle
 * @param the callback function to print the entry
 */
void
pq_dump(pqueue_t *q, 
		FILE *out,
		pqueue_print_entry print);


/**
 * checks that the pq is in the right order, etc
 * @internal
 * debug function only
 * @param q the queue
 */
int pq_is_valid(pqueue_t *q);


#endif /* PQUEUE_H */
/** @} */
