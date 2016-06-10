/*
 * NAME: Kyle Holmberg
 * DUCKID: kmh
 * SIN: 951312729
 * CLASS: CIS415
 *
 * Extra Credit Project
 * 
 * This file consists entirely of my original work
 */

#include <sys/types.h>
#include <dirent.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "linkedlist.h"
#include "treeset.h"
#include "re.h"
#include "workqueue.h"

#define _BSD_SOURCE 1 // Enables macros to test type of directory entry
#define MAXDATALEN 80  // For worker queue data
#define CRAWLER_THREADS 1 

void *build_workqueue_list (void *);

/*
 * routine to convert bash pattern to regex pattern
 * 
 * e.g. if bashpat is "*.c", pattern generated is "^.*\.c$"
 *      if bashpat is "a.*", pattern generated is "^a\..*$"
 *
 * i.e. '*' is converted to ".*"
 *      '.' is converted to "\."
 *      '?' is converted to "."
 *      '^' is put at the beginning of the regex pattern
 *      '$' is put at the end of the regex pattern
 *
 * assumes 'pattern' is large enough to hold the regular expression
 */
static void cvtPattern(char pattern[], const char *bashpat) 
{
	char *p = pattern;

	*p++ = '^';
	while (*bashpat != '\0') {
		switch (*bashpat) {
	      	case '*':
				*p++ = '.';
				*p++ = '*';
				break;

			case '.':
				*p++ = '\\';
				*p++ = '.';
				break;

			case '?':
				*p++ = '.';
				break;

			default:
				*p++ = *bashpat;
		}
		bashpat++;
	}
	*p++ = '$';
	*p = '\0';
}

/*
 * recursively opens directory files
 *
 * if the directory is successfully opened, it is added to the linked list
 *
 * for each entry in the directory, if it is itself a directory,
 * processDirectory is recursively invoked on the fully qualified name
 *
 * if there is an error opening the directory, an error message is
 * printed on stderr, and 1 is returned, as this is most likely indicative
 * of a protection violation
 *
 * if there is an error duplicating the directory name, or adding it to
 * the linked list, an error message is printed on stderr and 0 is returned
 *
 * if no problems, returns 1
 */
static int processDirectory(char *dirname, LinkedList *ll, int verbose) 
{
	DIR *dd;
	struct dirent *dent;
	char *sp;
	int len, status = 1;
	char d[4096];

	//eliminate trailing slash, if there
	strcpy(d, dirname);
	len = strlen(d);
	if (len > 1 && d[len-1] == '/')
		d[len-1] = '\0';

	// Open the directory
	if ((dd = opendir(d)) == NULL) {
		if (verbose)
			fprintf(stderr, "Error opening directory `%s'\n", d);
		return 1;
	}

	//Duplicate directory name to insert into linked list
	sp = strdup(d);
	if (sp == NULL) {
		fprintf(stderr, "Error adding `%s' to linked list\n", d);
		status = 0;
		goto cleanup;
	}

	if (!ll_add(ll, sp)) {
		fprintf(stderr, "Error adding `%s' to linked list\n", sp);
		free(sp);
		status = 0;
		goto cleanup;
	}

	if (len == 1 && d[0] == '/')
		d[0] = '\0';

	//read entries from the directory
	while (status && (dent = readdir(dd)) != NULL) {
		if (strcmp(".", dent->d_name) == 0 || strcmp("..", dent->d_name) == 0)
			continue;
		if (dent->d_type & DT_DIR) {
			char b[4096];
			sprintf(b, "%s/%s", d, dent->d_name);
			status = processDirectory(b, ll, 0);
		}
	}
cleanup:
	(void) closedir(dd);
	return status;
}

/*
 * comparison function between strings
 *
 * need this shim function to match the signature in treeset.h
 */
static int scmp(void *a, void *b) 
{
	return strcmp((char *)a, (char *)b);
}

/*
 * applies regular expression pattern to contents of the directory
 *
 * for entries that match, the fully qualified pathname is inserted into
 * the treeset
 */
static int applyRe(char *dir, RegExp *reg, TreeSet *ts) 
{
	DIR *dd;
	struct dirent *dent;
	int status = 1;

	//open the directory
	if ((dd = opendir(dir)) == NULL) {
		fprintf(stderr, "Error opening directory `%s'\n", dir);
		return 0;
	}

	//for each entry in the directory
	while (status && (dent = readdir(dd)) != NULL) {
		if (strcmp(".", dent->d_name) == 0 || strcmp("..", dent->d_name) == 0)
			continue;
		if (!(dent->d_type & DT_DIR)) {
			char b[4096], *sp;

	 		//see if filename matches regular expression
			if (! re_match(reg, dent->d_name))
				continue;
			sprintf(b, "%s/%s", dir, dent->d_name);
			
			//duplicate fully qualified pathname for insertion into treeset
			if ((sp = strdup(b)) != NULL) {
				if (!ts_add(ts, sp)) {
					fprintf(stderr, "Error adding `%s' to tree set\n", sp);
					free(sp);
					status = 0;
					break;
				}
			} else {
				fprintf(stderr, "Error adding `%s' to tree set\n", b);
				status = 0;
				break;
			}
		}
	}
	(void) closedir(dd);
	return status;
}

int main(int argc, char *argv[]) 
{
	int status;

	/* Linked List declarations for both queues */
	long complete_ll_size = 0; //debugging
	int set_num_worker_threads = CRAWLER_THREADS;
	LinkedList *initial_queue = NULL;
	workqueue_llist_t workqueue_ll;
	int maxlen = MAXDATALEN;
	// worker_group_struct my_very_own_worker_group;
	// NOTE> to simplify testing -- ONLY a Single Dir Arg is being processed 

	TreeSet *ts = NULL; //To contain matched bash-pattern-matched filenames
	char *sp;
	char pattern[4096];
	RegExp *reg;
	Iterator *it;

	if (argc < 2) {
		fprintf(stderr, "Usage: ./fileCrawler pattern [dir] ...\n");
		return -1;
	}
	
	printf("fileCrawler: %s -- processing dirtree [ %s ] ...\n", argv[0], argv[1]);
	printf("fileCrawler: %s -- processing with number of worker threads :=  %d \n\n", argv[0], set_num_worker_threads);


	//convert bash expression to regular expression and compile
	/*
	cvtPattern(pattern, argv[1]);
	if ((reg = re_create()) == NULL) {
		fprintf(stderr, "Error creating Regular Expression Instance\n");
		return -1;
	}
	if (! re_compile(reg, pattern)) {
		char eb[4096];
		re_status(reg, eb, sizeof eb);
		fprintf(stderr, "Compile error - pattern: `%s', error message: `%s'\n", pattern, eb);
		re_destroy(reg);
		return -1;
	}
	*/

	//create linked list and treeset
	if ((initial_queue = ll_create()) == NULL) {
		fprintf(stderr, "Unable to create initial_queue linked list\n");
		goto done;
	}
	if ((ts = ts_create(scmp)) == NULL) {
		fprintf(stderr, "Unable to create tree set\n");
		goto done;
	}

	//populate linked list
	if (argc == 2) {
		if (! processDirectory(".", initial_queue, 1))
			goto done;
	} else {
		int i;
		for (i = 2; i < argc; i++) {
			if (! processDirectory(argv[i], initial_queue, 1))
				goto done;
		}
	}

	//DEBUG -- Determine Final linkedlist Node Count = Size
    complete_ll_size = ll_size(initial_data_queue);
    printf("multiThreadFileCrawler> main() > complete_ll_size %ld !\n", complete_ll_size);

    //testing only
    pthread_t thread1, thread2;
  	int smallint1 = 2;
  	int smallint2 = 3;

  	workqueue_llist_init(&workqueue_ll);

  	// DEBUG -- List Current Contents of nitial_data_queue
    long idq_index = 1;
    // char *test_stringp;
    void **test_stringp;
    test_stringp = malloc(maxlen);
    char ll_node_data[256];
    // strcpy(ll_node_data, *test_stringp);
    long ll_idx = 0;
    printf("multiThreadFileCrawler> main() > outputing data for initial_data_queue\n\n");

    /*
    for(ll_idx = 0; ll_idx < complete_ll_size; ll_idx++) {

    	ll_get(initial_data_queue, idq_index, *test_stringp);

    	// strcpy(ll_node_data, *test_stringp);

    	printf ("initial_data_queue: Index: %ld\t Data: %s \n", ll_idx, (char **)test_stringp); // ll_node_data

	}
	*/

    /* ******************** */
    /* *** ADD BACK IN? *** */
    /* ******************** */
	//for each directory in the linked list, apply regular expression
	/*
	while (ll_removeFirst(initial_queue, (void **)&sp)) {
		int stat = applyRe(sp, reg, ts);
		free(sp);
		if (! stat) break;
	}
	*/

	//create iterator to traverse files matching pattern in sorted order
	/*
	if ((it = ts_it_create(ts)) == NULL) {
		fprintf(stderr, "Unable to create iterator over tree set\n");
		goto done;
	}

	while (it_hasNext(it)) {
		char *s;
		(void) it_next(it, (void **)&s);
		printf("%s\n", s);
	}

	it_destroy(it);
	*/

	/* 
	 *PROCESS linked list via worker threads-- 
	 * NOTE> to simplify testing -- ONLY a Single Dir Arg is being processed  
	 */
  	

  	//TEST THREADS
	/*
  	pthread_create(&thread1, 
		NULL,
		build_workqueue_list,
		(void *) &smallint1); 

  	pthread_create(&thread2, 
		NULL, 
		build_workqueue_list,
		(void *) &smallint2); 
  
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);

	workqueue_llist_show(&workqueue_ll);
	*/


	/* 
	/* NOTE -- Create and Start Worker Threads -- FIRST Test with a single worker thread and main-thread */
	/*
	status = worker_create (&my_very_own_worker_group, CRAWLER_THREADS);
    if (status != 0)
        err_abort (status, "ABORT -- Create Worker Group");

    // T.B.D / T.B.R.
    status = worker_group_start (&my_very_own_worker_group, argv[1]);
    // if (status != 0)
    //    err_abort (status, "Start worker_group");
	*/

//cleanup after ourselves so there are no memory leaks
done:
	if (initial_queue != NULL) 
		ll_destroy(initial_queue, free);
	if (ts != NULL) //resultant tree set
		ts_destroy(ts, free);
	//re_destroy(reg);
	return 0;
}
// 
//END MAIN
//


/* THIS IS DEBUG CODE Adapted from a TextBook
 * build_list -- makes entries to the linked list
 *               using multiples of the input arg
 
void *build_workqueue_list (void *arg) {

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
*/