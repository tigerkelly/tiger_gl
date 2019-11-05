
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "tiger_gl.h"

LList *llCreate() {
	LList *list = NULL;

	list = (LList *)calloc(1, sizeof(LList));
	list->inUse = 1;

	if (pthread_mutex_init(&(list->llock), NULL) != 0) {
		printf("Mutex init lock failed.\n");
		free(list);
		return NULL;
	}

	if (pthread_cond_init(&(list->lcond), NULL) != 0) {
		printf("Mutex condition init failed.\n");
		free(list);
		return NULL;
	}

	return list;
}

/*
 * This function addQdata is private to this file.
 *
 *   que = LList entry to add data to
 *   qData = Qdata structure to add to queue.
 *
 *   returns void
 */
void addLdata(LList *lst, Ldata *lData) {

	if (lst == NULL)
		return;

	if (lst->head == NULL) {
		lst->head = lst->tail = lData;
	} else {
		lst->tail->next = lData;
		lst->tail = lData;
	}
}

/*
 * This function llAdd adds data to the named queue
 *
 *   llName = LList name
 *   data = data to add to queue
 *   length = Length of data
 *
 *   return -1 on error
 *          0 on success
 */
int llAdd(LList *lp, const void *data, const int dataSize) {
	int ret = -1;

	if (lp == NULL)
		return ret;

	if (lp->inUse == 1) {
		pthread_mutex_lock(&(lp->llock));
		Ldata *lData = (Ldata *)calloc(1, sizeof(Ldata));
		lData->dataSize = dataSize;
		lData->data = calloc(1, dataSize);
		memcpy((char *)lData->data, (char *)data, dataSize);

		addLdata(lp, lData);
		lp->itemCount++;
		pthread_cond_signal(&(lp->lcond));
		ret = 0;

		pthread_mutex_unlock(&(lp->llock));
	}

	return ret;
}

/*
 * This function llRemove removes the first element in the queue and
 * returns the data to you.
 * NOTE: The data returned MUST be freed by the caller.
 *
 *   llName = LList name
 *   length = Pointer to int to place length into.
 *   block = if true then block waiting on an item else return null if queue empty
 *
 *   returns NULL on error or empty queue
 *           else pointer to data
 */
void *llRemove(LList *lp, bool block, int *dataSize) {
    void *data = NULL;
	Ldata *h = NULL;
	Ldata *t = NULL;

	if (lp == NULL)
		return NULL;

	if (lp->inUse == 1) {
		pthread_mutex_lock(&(lp->llock));

		if (lp->itemCount <= 0 && block == false) {
			pthread_mutex_unlock(&(lp->llock));
			return data;
		}

		while (lp->itemCount == 0) {
			pthread_cond_wait(&(lp->lcond), &(lp->llock));
		}

		h = lp->head;
		t = h->next;
		data = h->data;
		if (dataSize != NULL)
			*dataSize = h->dataSize;
		lp->head = t;
		if (lp->itemCount > 0)
			lp->itemCount--;
		else
			lp->itemCount = 0;

		pthread_mutex_unlock(&(lp->llock));
	}

	return data;
}

/* This function sllDestroy frees up the memory used by the given named list.
 *
 *  sllName = list name
 *
 *  return -1 on error
 *         else 0 on success
 */
int llDestroy(LList *lp) {

	if (lp == NULL)
		return -1;

	if (lp->inUse == 1) {
		pthread_mutex_lock(&(lp->llock));

		if (lp->data != NULL)
			free(lp->data);

		Ldata *ld = lp->head;
		while (ld != NULL) {
			Ldata *t = ld->next;
			free(ld);
			ld = t;
		}

		lp->inUse = 0;

		pthread_mutex_unlock(&(lp->llock));

		free(lp);
	}

	return 0;
}

/*
 * This function llCount returns the number of item in the named queue.
 *
 *   llName = LList name
 *
 *   returns number items in queue.
 */
int llCount(LList *lp) {
	int count = 0;

	if (lp == NULL)
		return -1;

	if (lp->inUse == 1) {
		pthread_mutex_lock(&(lp->llock));

		count = lp->itemCount;

		pthread_mutex_unlock(&(lp->llock));
	}

	return count;
}

#if(0)
/*
 * This function llList returns a comma separated string of the linked list.
 *
 *  llName = LList name
 *
 *  returns number items in queue.
 */
void *llList(LList *lp, int *length) {

	if (lp == NULL)
		return NULL;

	if (lp->inUse == 1) {
		pthread_mutex_lock(&(lp->llock));

		if (lp->tw != NULL)
			free(lp->tw);

		if (length != NULL)
			*length = lp->itemCount;

		lp->tw = (TglWidget *)calloc(lp->itemCount + 1, sizeof(TglWidget));
		TglWidget *twp = lp->tw;

		Ldata *ld = lp->head;
		while (ld != NULL) {
			*twp++ = ld->tw;
			ld = ld->next;
		}

		pthread_mutex_unlock(&(lp->llock));
	}

    return lp->tw;
}
#endif
