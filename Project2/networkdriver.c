/*
 * NAME: Kyle Holmberg
 * DUCKID: kmh
 * SIN: 951312729
 * CLASS: CIS415
 *
 * Project 2
 * 
 * This file consists entirely of my original work
 */

#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#include "packetdescriptor.h"
#include "destination.h"
#include "pid.h"
#include "freepacketdescriptorstore.h"
#include "freepacketdescriptorstore__full.h"
#include "networkdevice.h"
#include "packetdescriptorcreator.h"
#include "networkdevice.h"
#include "BoundedBuffer.h"
#include "diagnostics.h"

static void* send_thread();
static void* receive_thread();

#define BUFFERSIZE 10

FreePacketDescriptorStore **fpds_ptr;
NetworkDevice *netdev;
BoundedBuffer *applicationBuffer[MAX_PID];
BoundedBuffer *sendQueue;
BoundedBuffer *recPool;

void init_network_driver(NetworkDevice *nd, 
						 void *mem_start, 
						 unsigned long mem_length, 
						 FreePacketDescriptorStore **fpds_ptr) 
{
	//Initiliaze Device
	netdev = nd; //Assign network device from argument.

	pthread_t sendThread;
	pthread_t receiveThread;

    /* Create Free Packet Descriptor Store */
	*fpds_ptr = create_fpds();

    /* Load FPDS with packet descriptors constructed from mem_start/mem_length */
	create_free_packet_descriptors(*fpds_ptr, mem_start, mem_length);

    /* Create buffers required by your thread[s] */ 
    int i;
	for(i = 0; i <= (MAX_PID + 1); i++) {
        applicationBuffer[i] = createBB(BUFFERSIZE);
	}
	sendQueue = createBB(BUFFERSIZE);
	recPool = createBB(BUFFERSIZE);

    /* Create Threads */
    pthread_create(&sendThread, NULL, send_thread, NULL);
	pthread_create(&receiveThread, NULL, receive_thread, NULL);

    /* Return the FPDS to the code that called it */
	//destroy_fpds(*fpds_ptr);
}

/* 
 * Method to send threads 
 */
static void* send_thread()
{
	PacketDescriptor *temppd;
	while(1) {
		temppd = (PacketDescriptor*)blockingReadBB(sendQueue);
		int sendLimit = 5; //Arbitrarily set
		int i;
		for(i = 0; i < sendLimit; i++) { // Attempt to send 5 times
			if((send_packet(netdev, temppd)) == 1) { // If received, break.
				DIAGNOSTICS("[DRIVER> Info: Sent a packet after %d tries", i);
				break;			
			}
			usleep(3);
		}
		if (nonblockingWriteBB(recPool, temppd) != 1) {
			if (nonblocking_put_pd(*fpds_ptr, temppd) != 1) {
				DIAGNOSTICS("DRIVER> Error? Failed to return Packet Descriptor to store");
			}
		}
	}
	free(temppd);
	return NULL;
}

/* 
 * Method to receive threads 
 */
static void* receive_thread()
{
	PacketDescriptor* current_pd;
	PacketDescriptor* filled_pd;
    PID procID;

    /* First, receive the thread as fast as possible to allow work to continue */
    blocking_get_pd(*fpds_ptr, &current_pd); // Receive and block to handle other threads/packets
    init_packet_descriptor(&current_pd); // Reset packet descriptor before registering it to device.
    register_receiving_packetdescriptor(netdev, &current_pd); //Register the packet with the device.
    await_incoming_packet(netdev); //Waits until pd filled with data.

    /* Thread receival complete: Handle PD */
    while(1) {
        filled_pd = current_pd; //Packet Descriptor is now filled.
		if (nonblockingReadBB(recPool, &current_pd) != 1) { //If there's nothing waiting in the receive pool
			if (nonblocking_get_packet(*fpds_ptr, &current_pd) != 1) { //If we fail to get pd from fpds
				DIAGNOSTICS("DRIVER> Error? Cannot acquire Packet Descriptor.");
			}
		    init_packet_descriptor(&current_pd); //Reset pd before registering it to netdev
		    register_receiving_packetdescriptor(netdev, &current_pd); //Register packet with netdev
	        procID = packet_descriptor_get_pid(&filled_pd); //Find PID for Indexing
		    if (nonblockingWriteBB(applicationBuffer[procID], filled_pd) != 1) {
				DIAGNOSTICS("[DRIVER> Warning: Application(%u) Packet Store full, discarding data.\n", procID);
				if (nonblockingWriteBB(recPool, filled_pd) != 1) { //Can't get the packet from the receive pool...
					if (nonblocking_put_pd(*fpds_ptr, filled_pd) != 1) { //Can't return packet to fpds
				    	DIAGNOSTICS("[DRIVER> Error? Cannot return Packet Descriptor to store\n");
					}
				}
				
		    }
		} else {
		    printf("[DRIVER> Warning: No replacement Packet Descriptor, discarding data.\n");
		    current_pd = filled_pd;
	            init_packet_descriptor(&current_pd);
	            register_receiving_packetdescriptor(netdev, &current_pd);
		}
    }
    return NULL;
}

/* 
 * Queue up packet descriptor for sending
 * Do not return until it has been successfully queued
 */
void blocking_send_packet(PacketDescriptor *pd) 
{
	blockingWriteBB(sendQueue, pd);
	return;   
} 

/*
 * If you are able to queue up packet descriptor immediately, do so and return 1
 * Otherwise, return 0 
 */
int nonblocking_send_packet(PacketDescriptor *pd) 
{
	return nonblockingWriteBB(sendQueue, pd);
} 

/* 
 * Wait until there is a packet for 'pid'. 
 * Return that packet descriptor to the calling application.
 */
void blocking_get_packet(PacketDescriptor **pd, PID pid) 
{
	*pd = blockingReadBB(applicationBuffer[pid]);
	return;
} 

/* 
 * If there is currently a waiting packet for 'pid', 
 * return that packet to the calling application and return 1
 * otherwise, return 0 for the value of the function 
 */
int nonblocking_get_packet(PacketDescriptor **pd, PID pid) 
{
	return nonblockingReadBB(applicationBuffer[pid], pd);
}
