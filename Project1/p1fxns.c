/*
 *  a potentially useful set of subroutines for use with CIS 415
 *  project 1
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>


#include "p1fxns.h"

static char *digits = "0123456789";

/* p1getline - return EOS-terminated character array from fd returns number of characters in buf, 0 if end of file */
int p1getline(int fd, char buf[], int size) {
    int i;
    char c;
    int max = size - 1; /* must leave room for EOS */

    i = 0;
    for (i = 0; i < max; i++) {
        if (read(fd, &c, 1) == 0)
            break;
        buf[i] = c;
        if (c == '\n') {
            i++;
            break;
        }
    }
    buf[i] = '\0';
    return i;
}

/* p1strchr - return the array index of leftmost occurrence of 'c' in 'buf' | return -1 if not found */
int p1strchr(char buf[], char c) {
    int i;

    for (i = 0; buf[i] != '\0'; i++)
        if (buf[i] == c)
            return i;
    return -1;
}

/*
 * p1getword - fetch next blank-separated word from buffer into word
 *  return value is index into buffer for next search or -1 if at end
 *  N.B. assumes that word[] is large enough to hold the next word
 */
static char *singlequote = "'";
static char *doublequote = "\"";
static char *whitespace = " \t";

int p1getword(char buf[], int i, char word[]) {
    char *tc, *p;

    /* skip leading white space */
    while(p1strchr(whitespace, buf[i]) != -1)
        i++;
    /* buf[i] is now '\0' or a non-blank character */
    if (buf[i] == '\0')
        return -1;
    p = word;
    switch(buf[i]) {
    case '\'': tc = singlequote; i++; break;
    case '"': tc = doublequote; i++; break;
    default: tc = whitespace; break;
    }
    while (buf[i] != '\0') {
        if (p1strchr(tc, buf[i]) != -1)
            break;
        *p++ = buf[i];
        i++;
    }
    /* either at end of string or have found one of the terminators */
    if (buf[i] != '\0') {
        if (tc != whitespace) {
            i++;    /* skip over terminator */
        }
    }
    *p = '\0';
    return i;
}

/* p1strlen - return length of string */
int p1strlen(char *s) {
    char *p = s;

    while (*p != '\0')
        p++;
    return (p - s);
}

/* p1strdup - duplicate string on heap */
char *p1strdup(char *s) {
    int n = p1strlen(s) + 1;
    char *p = (char *)malloc(n);
    int i;

    if (p != NULL) {
        for (i = 0; i < n; i++)
            p[i] = s[i];
    }
    return p;
}

/* p1putint - display integer in decimal on file descriptor */
void p1putint(int fd, int number) {
    char buf[25];
    int n, i;

    if (number == 0) {
        buf[0] = '0';
        i = 1;
    } else {
        for (n = number, i = 0; n != 0; i++) {
            buf[i] = digits[n % 10];
            n /= 10;
        }
    }
    while (--i >= 0)
        write(fd, buf+i, 1);
    fsync(fd);
}

/* p1putstr - display string on file descriptor */
void p1putstr(int fd, char *s) {
    int n = p1strlen(s);
    write(fd, s, n);
    //while (*s != '\0') {
        //write(fd, s, 1);
        //s++;
    //}
    fsync(fd);
}

/* p1perror(int fd, char *str) - writes 'str' and string describing the last error on 'fd' */
void p1perror(int fd, char *str) {
    char *p = strerror(errno);

    p1putstr(fd, str);
    p1putstr(fd, " - ");
    p1putstr(fd, p);
    p1putstr(fd, "\n");
}

/* p1strcat(char *dest, char *src) - Concatenate characters onto a character array */
void p1strcat(char *dest, char *src) {
    int len = p1strlen(dest);
    int srclen = p1strlen(src);
    for (int i = 0; i <= srclen; i++) {
        dest[len+i] = src[i];
    }
}

/* p1itoa(int n) - convert n to characters */
char *p1itoa(int n) {
    char buf[25];
    int x;
    int i;
    
    if (n == 0) {
        buf[0] = '0';
        i = 1;
    } else {
        for (x = n, i = 0; x != 0; i++) {
            buf[i] = digits[x % 10];
            x /= 10;
        }
    }

    static char conversion[32];
    conversion[i] = '\0';

    for (int j = 0; i > 0; i--, j++) {
        conversion[i-1] = buf[j];
    }
    
    return conversion;
}