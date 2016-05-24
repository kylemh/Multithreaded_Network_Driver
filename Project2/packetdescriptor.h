#ifndef __PACKET_DESCRIPTOR_HDR
#define __PACKET_DESCRIPTOR_HDR

/*
 * Author:    Peter Dickman
 * Revised by Joe Sventek
 * Version:   1.4
 * Last edit: 2003-02-25
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

#include "destination.h"
#include "pid.h"

typedef struct packet_descriptor PacketDescriptor;

void init_packet_descriptor(PacketDescriptor *pd);
/* Resets the packet descriptor to be empty.        */
/* Should be used before registering a descriptor   */
/* with the NetworkDevice.                          */

/* The next few functions are used to set and get the destination info     */
/* Packets are routed to a Destination and then to the indicated PID there */

void packet_descriptor_set_pid(PacketDescriptor *pd, PID pid);
void packet_descriptor_set_destination(PacketDescriptor *pd, Destination d);

PID packet_descriptor_get_pid(PacketDescriptor *pd);
Destination packet_descriptor_get_destination(PacketDescriptor *pd);

/* Routines for manipulating the actual data are omitted, we don't bother */
/* sending actual data in this testharness.........                       */

#endif /* __PACKET_DESCRIPTOR_HDR */
