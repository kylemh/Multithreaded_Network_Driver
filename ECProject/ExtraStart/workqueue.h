/*
 * NAME: Kyle Holmberg
 * DUCKID: kmh
 * SIN: 951312729
 * CLASS: CIS415
 *
 * Extra Credit Project
 * 
 * This file consists entirely of my original work 
 * with the exception of following a thread-supported 
 * linked list implementation outlined in "Using POSIX 
 * Threads: Programming with Pthreads" by Brad Nichols
 */

#include <pthread.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef struct workqueue_llist_node {
	int index;
	void *datap;
	struct workqueue_llist_node *nextp;
} workqueue_llist_node_t;

typedef struct workqueue_llist { 
	workqueue_llist_node_t *first;
	pthread_mutex_t mutex;
} workqueue_llist_t;

int workqueue_llist_init (workqueue_llist_t *workqueue_llistp);
int workqueue_llist_insert_data (int index, void *datap, workqueue_llist_t *llistp);
int workqueue_llist_remove_data(int index, void **datapp, workqueue_llist_t *llistp);
int workqueue_llist_find_data(int index, void **datapp, workqueue_llist_t *llistp);
int workqueue_llist_change_data(int index, void *datap, workqueue_llist_t *llistp);
int workqueue_llist_show(workqueue_llist_t *llistp);
