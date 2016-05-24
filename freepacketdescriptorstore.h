#ifndef __FREE_PACKET_DESCRIPTOR_STORE_HDR
#define __FREE_PACKET_DESCRIPTOR_STORE_HDR

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

#include "packetdescriptor.h"

typedef struct free_packet_descriptor_store FreePacketDescriptorStore;

void blocking_get_pd(FreePacketDescriptorStore *fpds, PacketDescriptor **pd);
int  nonblocking_get_pd(FreePacketDescriptorStore *fpds, PacketDescriptor **pd);

void blocking_put_pd(FreePacketDescriptorStore *fpds, PacketDescriptor *pd);
int  nonblocking_put_pd(FreePacketDescriptorStore *fpds, PacketDescriptor *pd);

/* As usual, the blocking versions only return when they succeed. */
/* The nonblocking versions return 1 if they worked, 0 otherwise. */
/* The _get_ functions set their final arg if they succeed.       */

#endif /* __FREE_PACKET_DESCRIPTOR_STORE_HDR */
