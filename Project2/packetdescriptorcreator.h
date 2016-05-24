#ifndef __CREATE_FREE_PACKET_DESCRIPTORS_HDR
#define __CREATE_FREE_PACKET_DESCRIPTORS_HDR

/*
 * Author:    Peter Dickman
 * Revised by Joe Sventek
 * Version:   1.4
 * Last edit: 2003-02-18
 * Edited: 4 May 2015
 *
 * This file is a component of the test harness and or sample
 * solution to the NetworkDriver exercise (re)developed in May 20154
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


#include "freepacketdescriptorstore.h"

/*
 *	uses the mem_length bytes starting at mem_start to pupulate fpds
 *	with packet descriptors
 *
 *	returns the number of packet descriptors as the value of the function
 */

int create_free_packet_descriptors(FreePacketDescriptorStore *fpds,
				    void                      *mem_start,
				    unsigned long             mem_length);

#endif /* __CREATE_FREE_PACKET_DESCRIPTORS_HDR */
