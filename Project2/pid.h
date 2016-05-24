#ifndef __PID_HDR
#define __PID_HDR

/*
 * Author:    Peter Dickman
 * Revised by Joe Sventek
 * Version:   1.4
 * Last edit: 2003-02-18
 * Edited: 4 May 2015
 *
 * This file is a component of the test harness and or sample
 * solution to the NetworkDriver exercise (re)developed in May 2015
 * for use in assessing CIS 415 Spring 2015
 *
 * It tests the ability to develop a small but complex software system
 * using PThreads to provide concurrency in C.
 *
 * Copyright:
 * (c) 2003 University of Glasgow and Dr Peter Dickman
 *
 * this software used with the author's permission.
 *
 */

typedef unsigned int PID;
#define MAX_PID 10
/* A PID is used to distinguish between the different applications */
/* It is implemented as an unsigned int, but is guaranteed not to  */
/* exceed MAX_PID.                                                 */

#endif /* __PID_HDR */
