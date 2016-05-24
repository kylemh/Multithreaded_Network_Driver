#ifndef __BOUNDED_BUFFER_HDR
#define __BOUNDED_BUFFER_HDR

/*
 * BoundedBuffer.h
 *
 * Header file for a bounded buffer implemented using pthread
 * facilities to allow it to be driven by concurrent threads.
 *
 * Uses standard tricks to keep it very generic.
 * Uses heap allocated data structures.
 *
 * Author: Peter Dickman
 * Revised by Joe Sventek
 *
 * Created: 7-3-2000
 * Edited:  28-2-2001
 * Edited:  4 May 2015
 *
 * Version: 1.2
 *
 */

typedef struct bounded_buffer BoundedBuffer;

BoundedBuffer *createBB(int size);
void destroyBB(BoundedBuffer *bb);

void blockingWriteBB(BoundedBuffer *bb, void *item);
void *blockingReadBB(BoundedBuffer *bb);

int nonblockingWriteBB(BoundedBuffer *bb, void *item);
int nonblockingReadBB(BoundedBuffer *bb, void **item);

#endif /* __BOUNDED_BUFFER_HDR */
