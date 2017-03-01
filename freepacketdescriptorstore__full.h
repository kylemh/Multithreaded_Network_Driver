#ifndef __FREE_PACKET_DESCRIPTOR_STORE__FULL_HDR
#define __FREE_PACKET_DESCRIPTOR_STORE__FULL_HDR

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

#include "freepacketdescriptorstore.h"

/* This header adds the create/destroy calls to the specified behaviour */

FreePacketDescriptorStore *create_fpds(void);
void destroy_fpds(FreePacketDescriptorStore *fpds);

#endif /* __FREE_PACKET_DESCRIPTOR_STORE__FULL_HDR */
