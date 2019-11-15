/*
 * cqueue.c
 *
 * A circular fifo queue.
 *
 *  Created on: Nov 6, 2016
 *      Author: Kelly Wiles
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdatomic.h>
#include <errno.h>

#include "tiger_gl.h"
#include "myatomic.h"

#define MILLION		1000000L
#define BILLION		1000000000L

int _maxCQueues;
int _currCQCount;
CQueue **_cqueues = NULL;

pthread_mutex_t _cqueueLock = PTHREAD_MUTEX_INITIALIZER;

int cqInit(int cqueueCnt) {
	if (cqueueCnt == 1)
		_maxCQueues = 2;
	else if (cqueueCnt <= 0)
		_maxCQueues = MAX_CQUEUES;
	else
		_maxCQueues = cqueueCnt;
	_currCQCount = 0;

//	printf("cqueueCnt: %d\n", cqueueCnt);

	// Allocate pointer array to hold each CQueue created with cqCreate()
	_cqueues = (CQueue **)calloc(_maxCQueues, sizeof(CQueue *));

	if (pthread_mutex_init(&_cqueueLock, NULL) != 0) {
		printf("Error: Mutex init lock failed.\n");
		return -1;
	}

	return 0;
}

/*
 * This function cqCreate creates a named Linked List Queue
 *
 *   cqName = Queue name to create.
 *   arrSize = Size of number of messages
 *
 * NOTE: The max size of the queue will be able to handle is (arrSize - 1)
 * 		 This function does not allow for growth if queue is full.
 *
 *   returns -1 on error
 *           else 0 or greater on success
 */
int cqCreate(const char *cqName, int arrSize) {
	return cqCreateDynamic(cqName, arrSize, 0);
}

/*
 * This function cqCreateDynamic creates a named Linked List Queue
 * Also this function will grow queue if it is full.
 * It will never shrink the queue.
 *
 *   cqName = Queue name to create.
 *   arrSize = Size of number of messages
 *   growth = how much to grow queue if full, 0 means do not grow.
 *
 * NOTE: The max size of the queue will be able to handle is (arrSize - 1)
 *
 *   returns -1 on error
 *           else 0 or greater on success
 */
int cqCreateDynamic(const char *cqName, int arrSize, int growth) {

	if (_cqueues == NULL) {
		printf("Error: Must call cqInit() first.\n");
		return -1;
	}
	if (_currCQCount >= _maxCQueues) {
		printf("Error: No more CQ structures available\n");
		return -1;
	}
    _currCQCount++;

	pthread_mutex_lock(&_cqueueLock);

	// Look for empty slot or an exiting entry.
	CQueue *qp;
	int empty = -1;
	for (int i = 0; i < _maxCQueues; i++) {
		qp = _cqueues[i];
		if (qp == NULL) {
			empty = i;
			continue;
		}
		// already been created ?
		if (strcmp(qp->cqName, cqName) == 0) {
			pthread_mutex_unlock(&_cqueueLock);
			return i;
		}
	}

	if (empty == -1) {
		pthread_mutex_unlock(&_cqueueLock);
		printf("Error: No free slots.\n");
		return -1;
	}

	int size = arrSize * sizeof(ItemType);

//	printf("Info: size: %d, %d\n", size, (sizeof(CQueue) + size));

	CQueue *cp = (CQueue *)calloc(1, (sizeof(CQueue) + size));
	if (cp == NULL) {
		printf("Error: Unable to allocate memory for CQueue structure\n");
		pthread_mutex_unlock(&_cqueueLock);
		return -1;
	}

	_cqueues[empty] = cp;

//	printf("Info: %p, %p\n", _cqueues[empty], cp);

	pthread_mutex_init(&cp->cqLock, NULL);
	pthread_cond_init(&cp->cqCond, NULL);
//	cp->cqCond = PTHREAD_COND_INITIALIZER;
//	cp->cqLock = PTHREAD_MUTEX_INITIALIZER;

	if (pthread_mutex_init(&cp->cqLock, NULL) != 0) {
		printf("Error: Mutex init lock failed.\n");
		pthread_mutex_unlock(&_cqueueLock);
		return -1;
	}

	if (pthread_cond_init(&cp->cqCond, NULL) != 0) {
		printf("Error: Mutex condition init failed.\n");
		pthread_mutex_unlock(&_cqueueLock);
		return -1;
	}

	cp->cqItemCount = 0;
	cp->queIn = 0;
	cp->queOut = arrSize -1;
	cp->arrSize = arrSize;
	cp->growth = growth;
	strcpy(cp->cqName, cqName);

	pthread_mutex_unlock(&_cqueueLock);

	return empty;
}

/*
 * If the queue is used to access an array of memory
 * then this will allow the array to be reallocated
 * if the queue ever grows in size.
 *
 *   cqNum = Number returned by the cqCreate() or cqQueNum() functions
 *   buffer = pointer to memory array.
 *   blkSize = size of an array element.
 *
 */
int cqSetBuffer(int cqNum, void *buffer, int blkSize) {

	if (_cqueues == NULL) {
		printf("Error: Must call cqInit() first.\n");
		return -1;
	}

	CQueue *cp = _cqueues[cqNum];

	cp->buffer = buffer;
	cp->blkSize = blkSize;

	return 0;
}

/*
 * Used to change the number of elements to grow by.
 *
 *  cqNum = Number returned by the cqCreate() or cqQueNum() functions
 *  growth = number of elements to grow queue by.
 *
 * Can set the growth to zero to stop any future growth.
 */
int cqSetGrowth(int cqNum, int growth) {

	if (_cqueues == NULL) {
		printf("Error: Must call cqInit() first.\n");
		return -1;
	}

	CQueue *cp = _cqueues[cqNum];

	if (growth < 0)
		cp->growth = 0;
	else
		cp->growth = growth;

	return 0;
}

/*
 * Grows the queue and the buffer associated with it if any..
 *
 *   cqNum = Number returned by the cqCreate() or cqQueNum() functions
 *   growth = number of elements to grow queue by.
 */
int cqGrow(int cqNum, int growth) {

	if (_cqueues == NULL) {
		printf("Error: Must call cqInit() first.\n");
		return -1;
	}

	if (growth <= 0)
		return -1;

	CQueue *cp = _cqueues[cqNum];

	pthread_mutex_lock(&cp->cqLock);

	int newSize = (cp->arrSize + growth) * sizeof(ItemType);
	CQueue *p = (CQueue *)realloc(cp, (sizeof(CQueue) + newSize));
	if (p != NULL) {
		printf("Info: Grow queue by %d elements.\n", growth);
		// Success in reallocating memory.
		_cqueues[cqNum] = p;

		for (int k = cp->arrSize; k < (cp->arrSize + growth); k++) {
			cp->array[k].l = 0;		// initialize what we added.
		}

		cp->arrSize += growth;
		cp->growth = growth;
		cp->queOut = cp->arrSize - 1;

		if (cp->buffer != NULL && cp->blkSize > 0) {
			void *p2 = (void *)realloc(*cp->buffer, (cp->arrSize * cp->blkSize));
			if (p2 != NULL) {
				cp->buffer = &p2;
			}
		}
	}

	pthread_mutex_unlock(&cp->cqLock);

	return 0;
}

/*
 * This function cqAdd adds a value to the circular list.
 *
 *   cqNum = Number returned by the cqCreate() or cqQueNum() functions
 *   value = The value to place into the circular list.
 *
 *   return 0 on success
 *   		-3 failed to reallocate memory, queue still full
 *          -2 on queue full
 *   		-1 on error;
 */
int cqAdd(int cqNum, ItemType *value) {

	if (_cqueues == NULL) {
		printf("Error: Must call cqInit() first.\n");
		return -1;
	}

	CQueue *cp = _cqueues[cqNum];

	pthread_mutex_lock(&cp->cqLock);

	if (cp->queIn == cp->queOut) {
		// queue is full.
		if (cp->growth > 0) {
			// Grow Queue.
			int newSize = (cp->arrSize + cp->growth) * sizeof(ItemType);
			CQueue *p = (CQueue *)realloc(cp, (sizeof(CQueue) + newSize));
			if (p != NULL) {
				printf("Info: Grow queue by %d elements.\n", cp->growth);
				// Success in reallocating memory.
				_cqueues[cqNum] = p;

				for (int k = cp->arrSize; k < (cp->arrSize + cp->growth); k++) {
					cp->array[k].l = 0;		// initialize what we added.
				}

				cp->arrSize += cp->growth;
				cp->queOut = cp->arrSize - 1;

				if (cp->buffer != NULL && cp->blkSize > 0) {
					void *p2 = (void *)realloc(*cp->buffer, (cp->arrSize * cp->blkSize));
					if (p2 != NULL) {
						cp->buffer = &p2;
					}
				}
			} else {
				// failed to reallocate memory.
				printf("Error: Failed to reallocate memory.\n");
				pthread_cond_broadcast(&cp->cqCond);
				pthread_mutex_unlock(&cp->cqLock);
				return -3;
			}
		} else {
			pthread_cond_broadcast(&cp->cqCond);
			pthread_mutex_unlock(&cp->cqLock);
			return -2;
		}
	}

	*(ItemType *)&cp->array[cp->queIn] = *value;
//	memcpy(&cp->array[cp->queIn], value, sizeof(ItemType));

	// move queIn to next slot.
	cp->queIn = (cp->queIn + 1) % cp->arrSize;
	cp->cqItemCount++;
//	printf("printf: cqAdd %s %d\n", cqGetName(cqNum), cp->cqItemCount);
	pthread_cond_broadcast(&cp->cqCond);

	pthread_mutex_unlock(&cp->cqLock);

	return 0;
}

/*
 * This function cqRemove returns the first element in the queue.
 *
 *   cqNum = Queue index
 *   value = Place value from queue here.
 *   block = if true then block waiting on an item else return null if queue empty
 *   ms = Timeout in milliseconds, if CQ_TIMERBLOCK is used.
 *
 *   returns -1 on error or empty queue
 *           else 0
 */
int cqRemove(int cqNum, ItemType *value, int block) {
	int ret = -1;

	if (_cqueues == NULL) {
		printf("Error: Must call cqInit() first.\n");
		return ret;
	}

	CQueue *cp = _cqueues[cqNum];

	pthread_mutex_lock(&cp->cqLock);
	if (((cp->queOut + 1) % cp->arrSize) == cp->queIn) {
		// queue is empty.
		if (cp->cqItemCount <= 0 && block == CQ_NONBLOCK) {
			pthread_mutex_unlock(&cp->cqLock);
//			printf("Error: Queue empty and non blocking is turned on.\n");
			return -1;
		}

		// block waiting on data to arrive.
//		printf("Info: Waiting on queue %s  %d\n", cqGetName(cqNum), _cqueues[cqNum]->cqItemCount);
		while (cp->cqItemCount == 0) {
			pthread_cond_wait(&cp->cqCond, &cp->cqLock);
		}
//		printf("Awake from queue. %s %d\n", cqGetName(cqNum), cp->cqItemCount);
	}

	cp->queOut = (cp->queOut + 1) % cp->arrSize;

	*value = *(ItemType *)&cp->array[cp->queOut];
//	memcpy(value, &cp->array[cp->queOut], sizeof(ItemType));

	int exp = -1;
	int d = 0;

	cp->cqItemCount--;
	if (AtomicExchange(&cp->cqItemCount, &exp, &d) == 1) {
		printf("Info: Warning: cp->cqItemCount was -1.\n");
	}

	pthread_mutex_unlock(&cp->cqLock);

	return 0;
}

/*
 * This function cqRemoveTimed returns the first element in the queue.
 *
 *   cqNum = Queue index
 *   value = Place value from queue here.
 *   timeout = Block N milliseconds waiting on an item.
 *
 *   returns -1 on error
 *   		 -2 timed out
 *           else 0
 */
int cqRemoveTimed(int cqNum, ItemType *value, int timeout) {
	int ret = -1;
	int rc;
	unsigned long t;
	struct timeval tv;			// microseconds 1 millionth of a second
	struct timespec ts;			// nanoseconds 1 billionth of a second

	t = timeout * MILLION;		// convert milliseconds to nanoseconds.

	if (_cqueues == NULL) {
		printf("Error: Must call cqInit() first.\n");
		return ret;
	}

	CQueue *cp = _cqueues[cqNum];

	pthread_mutex_lock(&cp->cqLock);
	if (((cp->queOut + 1) % cp->arrSize) == cp->queIn) {
		// queue is empty.

		gettimeofday(&tv, NULL);

		// Convert from timeval to timespec
		ts.tv_sec  = tv.tv_sec;
		ts.tv_nsec = tv.tv_usec * 1000;
		// add timeout (t) into timespec
		ts.tv_sec += (t / BILLION);
		ts.tv_nsec += (t % BILLION);

		// block waiting on data to arrive or time out.
		// printf("Waiting on queue %s  %d\n", cqGetName(cqNum), _cqueues[cqNum]->cqItemCount);
		while (cp->cqItemCount == 0) {
			rc = pthread_cond_timedwait(&cp->cqCond, &cp->cqLock, &ts);
			if (rc == ETIMEDOUT) {
				pthread_mutex_unlock(&cp->cqLock);
				return -2;
			}
		}
		// printf("Awake from queue. %s %d\n", cqGetName(cqNum), cp->cqItemCount);
	}

	cp->queOut = (cp->queOut + 1) % cp->arrSize;

	*value = *(ItemType *)&cp->array[cp->queOut];

	int exp = -1;
	int d = 0;

	cp->cqItemCount--;
	if (AtomicExchange(&cp->cqItemCount, &exp, &d) == 1) {
		printf("Info: Warning: cp->cqItemCount was -1.\n");
	}

	pthread_mutex_unlock(&cp->cqLock);

	return 0;
}

int cqDestroy(int queNum) {

	if (_cqueues == NULL) {
		printf("Error: Must call cqInit() first.\n");
		return -1;
	}

//	CQueue *cp = _cqueues[queNum];

	pthread_mutex_lock(&_cqueueLock);

	free(_cqueues[queNum]);
	_cqueues[queNum] = NULL;

	pthread_mutex_unlock(&_cqueueLock);

	return 0;
}

/*
 * This function cqCount return the number of item in the named queue.
 *
 *   cqName = CQueue name
 *
 *   returns number items in queue,
 *           -1 on error.
 */
int cqCount(int queNum) {
//	int count = 0;

	if (_cqueues == NULL) {
		printf("Error: Must call cqInit() first.\n");
		return -1;
	}

//	CQueue *cp = _cqueues[queNum];

//	pthread_mutex_lock(&cp->cqLock);
//	count = cp->cqItemCount;
//	pthread_mutex_unlock(&cp->cqLock);

	return _cqueues[queNum]->cqItemCount;
}

int cqArrSize(int queNum) {
	if (_cqueues == NULL) {
		printf("Error: Must call cqInit() first.\n");
		return -1;
	}

	CQueue *cp = _cqueues[queNum];

	return cp->arrSize;
}

char *cqGetName(int queNum) {
	char *name = NULL;

	if (_cqueues == NULL) {
		printf("Error: Must call cqInit() first.\n");
		return 0;
	}

	CQueue *cp = _cqueues[queNum];
	if (cp == NULL)
		return NULL;

	name = cp->cqName;

	return name;
}

/*
 * This function cqQueNum returns the index number for the given name.
 *
 *   cqName = Name to find index of.
 *
 * returns queue number or
 *         -1 on error
 */
int cqGetNum(char *cqName) {

	CQueue *cp;

	for (int i = 0; i < _maxCQueues; i++) {
		cp = _cqueues[i];
		if (cp == NULL)
			continue;
		if (strcmp(cp->cqName, cqName) == 0)
			return i;
	}

	return -1;
}
