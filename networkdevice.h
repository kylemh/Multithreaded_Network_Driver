#ifndef __NETWORK_DEVICE_HDRS
#define __NETWORK_DEVICE_HDRS

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

typedef struct network_device NetworkDevice;

int send_packet(NetworkDevice *nd, PacketDescriptor *pd);
/* Returns 1 if successful, 0 if unsuccessful */
/* May take a substantial time to return      */
/* If unsuccessful you can try again, but if you fail repeatedly give */
/* up and just accept that this packet cannot be sent for some reason */

void register_receiving_packetdescriptor(NetworkDevice *nd, PacketDescriptor *pd);
/* tell the network device to use the indicated PacketDescriptor     */
/* for next incoming data packet; once a descriptor is used it won't */
/* be reused for a further incoming data packet; you must register   */
/* another PacketDescriptor before the next packet arrives           */

void await_incoming_packet(NetworkDevice *nd);
/* The thread blocks until the registered PacketDescriptor has been   */
/* filled with an incoming data packet. The PID field of the packet   */
/* indicates the local application process which should be given it.  */
/* This should be called as soon as possible after the previous       */
/* call to register_receiving_packetdescriptor() to wait for a packet */
/* Only 1 thread may be waiting. */

#endif /* __NETWORK_DEVICE_HDRS */
