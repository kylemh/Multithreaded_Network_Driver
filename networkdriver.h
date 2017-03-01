#ifndef __NETWORK_DRIVER_HDR
#define __NETWORK_DRIVER_HDR

/*
 * Author:    Peter Dickman
 * Revised by Joe Sventek
 * Version:   1.5
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


#include "packetdescriptor.h"
#include "destination.h"
#include "pid.h"
#include "freepacketdescriptorstore.h"

#include "networkdevice.h"

/* These are the calls to be implemented by the students */

void blocking_send_packet(PacketDescriptor *pd);
int  nonblocking_send_packet(PacketDescriptor *pd);
/* These calls hand in a PacketDescriptor for dispatching */
/* The nonblocking call must return promptly, indicating whether or */
/* not the indicated packet has been accepted by your code          */
/* (it might not be if your internal buffer is full) 1=OK, 0=not OK */
/* The blocking call will usually return promptly, but there may be */
/* a delay while it waits for space in your buffers.                */
/* Neither call should delay until the packet is actually sent      */

void blocking_get_packet(PacketDescriptor **pd, PID);
int  nonblocking_get_packet(PacketDescriptor **pd, PID);
/* These represent requests for packets by the application threads */
/* The nonblocking call must return promptly, with the result 1 if */
/* a packet was found (and the first argument set accordingly) or  */
/* 0 if no packet was waiting.                                     */
/* The blocking call only returns when a packet has been received  */
/* for the indicated process, and the first arg points at it.      */
/* Both calls indicate their process number and should only be     */
/* given appropriate packets. You may use a small bounded buffer   */
/* to hold packets that haven't yet been collected by a process,   */
/* but are also allowed to discard extra packets if at least one   */
/* is waiting uncollected for the same PID. i.e. applications must */
/* collect their packets reasonably promptly, or risk packet loss. */

void init_network_driver(NetworkDevice               *nd, 
                         void                        *mem_start, 
                         unsigned long               mem_length,
                         FreePacketDescriptorStore **fpds_ptr);
/* Called before any other methods, to allow you to initialise */
/* data structures and start any internal threads.             */ 
/* Arguments:                                                  */
/*   nd: the NetworkDevice that you must drive,                */
/*   mem_start, mem_length: some memory for PacketDescriptors  */
/*   fpds_ptr: You hand back a FreePacketDescriptorStore into  */
/*             which you have put the divided up memory        */
/* Hint: just divide the memory up into pieces of the right size */
/*       passing in pointers to each of them                     */ 

#endif /* __NETWORK_DRIVER_HDR */
