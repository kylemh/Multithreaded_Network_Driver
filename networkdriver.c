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

int receiveCount = 0;
int sendCount = 0;

FreePacketDescriptorStore *free_pd_store;
NetworkDevice *netdev;
BoundedBuffer *applicationBuffers[MAX_PID];
BoundedBuffer *sendPool;
BoundedBuffer *recPool;


// definition[s] of functcion[s] required for your thread[s] */
void init_network_driver(NetworkDevice *nd, 
						 void *mem_start, 
						 unsigned long mem_length, 
						 FreePacketDescriptorStore **fpds_ptr) 
{
	DIAGNOSTICS("Info: Constructing network device...\n");
	netdev = nd; //Assign network device from argument.
	DIAGNOSTICS("Info: Constructed network device.\n");

	DIAGNOSTICS("Info: Initialising network driver...");
	pthread_t sendThread;
	pthread_t receiveThread;

    /* create Free Packet Descriptor Store */
	free_pd_store = create_fpds();
	*fpds_ptr = free_pd_store;
	DIAGNOSTICS("Info: Allocated memory for packet descriptor contents");

    /* load FPDS with packet descriptors constructed from mem_start/mem_length */
	create_free_packet_descriptors(free_pd_store, mem_start, mem_length);

    /* create any buffers required by your thread[s] */ 
	for(int i = 0; i <= (MAX_PID + 1); i++) {
        applicationBuffers[i] = createBB(BUFFERSIZE);
	}
	sendPool = createBB(BUFFERSIZE);
	recPool = createBB(BUFFERSIZE);

    /* create any threads you require for your implementation */
    pthread_create(&sendThread, NULL, send_thread, NULL);
	pthread_create(&receiveThread, NULL, receive_thread, NULL);

    /* return the FPDS to the code that called you */
	destroy_fpds(free_pd_store);
}

/* 
 * Method to send threads 
 */
static void* send_thread()
{
	PacketDescriptor *temppd;
	while(1) {
		temppd = (PacketDescriptor)blockingReadBB(sendPool);

		for(int i = 0; i < 5; i++) { // Attempt to send 5 times
			if((send_packet(netdev, temppd)) == 1) { // If received, break.
				DIAGNOSTICS("[DRIVER> Info: Sent a packet after %d tries", i);
				break;			
			}
			usleep(3);
		}
		blocking_put_pd(free_pd_store, temppd);
	}
	free(temppd);
	return NULL;
}

/* 
 * Method to receive threads 
 */
static void* receive_thread()
{
	//TODO: Try to get from pool first, else get from store
	PacketDescriptor *temppd;
	while(1) {
		if((nonblocking_get_pd(free_pd_store, &temppd)) == 1) { // Packet Descriptor acquired from free_pd_store
			init_packet_descriptor(&temppd); // Reset packet descriptor before registering it to netdev
			register_receiving_packetdescriptor(netdev, &temppd); // Tell device to register given packet

			await_incoming_packet(netdev); // Await connection confirmation from netdev

			PID packetIndex = packet_descriptor_get_pid(&temppd); 
			if((nonblockingWriteBB(applicationBuffers[packetIndex], temppd)) == 1) { //If packet writes...
				//
			} else {
				while(1) {
					if ((nonblockingWriteBB(applicationBuffers[packetIndex], temppd)) == 1) break; // Write successful - move on.
					if ((nonblocking_put_pd(free_pd_store, temppd)) == 1) break; // Give packet back to fpds
				}
			}
		} else {
			DIAGNOSTICS("[Device> packet receival failed.");
		}
	}
	free(temppd);
	return NULL;
}

/* 
 * Queue up packet descriptor for sending
 * Do not return until it has been successfully queued
 */
void blocking_send_packet(PacketDescriptor *pd) 
{
	blockingWriteBB(sendPool, pd);
	return;   
} 

/*
 * If you are able to queue up packet descriptor immediately, do so and return 1
 * Otherwise, return 0 
 */
int nonblocking_send_packet(PacketDescriptor *pd) 
{
	return nonblockingWriteBB(sendPool, pd);
} 

/* 
 * Wait until there is a packet for 'pid'. 
 * Return that packet descriptor to the calling application.
 */
void blocking_get_packet(PacketDescriptor **pd, PID pid) 
{
	*pd = blockingReadBB(recPool[pid]);
	return;
} 

/* 
 * If there is currently a waiting packet for 'pid', 
 * return that packet to the calling application and return 1
 * otherwise, return 0 for the value of the function 
 */
int nonblocking_get_packet(PacketDescriptor **pd, PID pid) 
{
	return nonblockingReadBB(recPool[pid], pd);
}
