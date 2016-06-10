#include <stdio.h>
#include <stdlib.h>
#include "workqueue.h"

#define MAXDATALEN 80

void *build_list (void *);

workqueue_llist_t ll; 
int maxlen=MAXDATALEN;

extern int
main(void) {
  
  pthread_t thread1, thread2;
  int smallint1 = 2;
  int smallint2 = 3;

  workqueue_llist_init(&ll);
 
  pthread_create(&thread1, NULL, build_list, (void *) &smallint1); 
  pthread_create(&thread2, NULL, build_list, (void *) &smallint2); 
  
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);

  workqueue_llist_show(&ll);

  return 0;
}


/*
 * build_list -- makes entries to the linked list
 *               using multiples of the input arg
 */
void *build_list (void *arg) {
  int *factorp = (int *)arg;
  unsigned int multiple;
  char *stringp;
  int i;

  for (i = 0; i < 1000; i++) {
    multiple = (*factorp) * i;
    stringp = malloc(maxlen);
    if (stringp == NULL) 
      perror("malloc");

    sprintf (stringp, "%d is a multiple of %d", multiple, *factorp);
    workqueue_llist_insert_data(multiple, stringp, &ll);
  }
  return NULL;
}


