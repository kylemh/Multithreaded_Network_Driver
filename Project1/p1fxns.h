/*
 *	a potentially useful set of subroutines for use with CIS 415
 *	project 1
 */

#ifndef _P1FXNS_H_
#define _P1FXNS_H_

/* p1getline - return EOS-terminated character array from fd returns number of characters in buf, 0 if end of file */
int p1getline(int fd, char buf[], int size);

/* p1strchr - return the array index of leftmost occurrence of 'c' in 'buf' | return -1 if not found */
int p1strchr(char buf[], char c);

/*
 * p1getword - fetch next blank-separated word from buffer into word
 *  return value is index into buffer for next search or -1 if at end
 *  N.B. assumes that word[] is large enough to hold the next word
 */
int p1getword(char buf[], int i, char word[]);

/* p1strlen - return length of string */
int p1strlen(char *s);

/* p1strdup - duplicate string on heap */
char *p1strdup(char *s);

/* p1putint - display integer in decimal on file descriptor */
void p1putint(int fd, int number);

/* p1putstr - display string on file descriptor */
void p1putstr(int fd, char *s);

/* p1perror(int fd, char *str) - writes 'str' and string describing the last error on 'fd' */
void p1perror(int fd, char *str);

void p1strcat(char *dest, char *src);

char *p1itoa(int n);

#endif	/* _P1FXNS_H_ */
