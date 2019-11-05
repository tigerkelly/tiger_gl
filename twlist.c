
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "tiger_gl.h"

TwList *twCreate() {
    TwList *list = NULL;

    list = (TwList *)calloc(1, sizeof(TwList));
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
 *   que = TwList entry to add data to
 *   qData = Qdata structure to add to queue.
 *
 *   returns void
 */
void _twAddTwData(TwList *lst, TwData *lData) {

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
 *   llName = TwList name
 *   data = data to add to queue
 *   length = Length of data
 *
 *   return -1 on error
 *          0 on success
 */
int twAdd(TwList *lp, TglWidget *tw) {
	int ret = -1;

	if (lp == NULL)
		return ret;

	if (lp->inUse == 1) {
		pthread_mutex_lock(&(lp->llock));
		TwData *lData = (TwData *)calloc(1, sizeof(TwData));
		lData->tw = tw;
		// printf("lData->tw %p, tw %p, %s, %d\n", lData->tw, tw, tw->text, tw->widgetType);

		_twAddTwData(lp, lData);
		lp->itemCount++;
		pthread_cond_signal(&(lp->lcond));
		ret = 0;

		pthread_mutex_unlock(&(lp->llock));
	}

	return ret;
}

/*
 * This function llFind finds the widget ID if present in the linked list.
 *
 *   llName = TwList name
 *   id = widget ID to search for in linked list.
 *
 *   returns 1 if ID is found
 *           else 0 if not found.
 */
int twFind(TwList *lp, int id) {
    int ret = 0;

	if (lp == NULL)
		return -1;

	if (lp->inUse == 1) {
		pthread_mutex_lock(&(lp->llock));

		if (lp->itemCount <= 0) {
			pthread_mutex_unlock(&(lp->llock));
			return ret;
		}

		TwData *ld = lp->head;
		for (int x = 0; x < lp->itemCount; x++, ld = ld->next) {
			if (ld->tw->widgetId == id) {
				ret = 1;
				break;
			}
		}

		pthread_mutex_unlock(&(lp->llock));
	}

	return ret;
}

/*
 * This function llRemove removes the first element in the queue and
 * returns the data to you.
 * NOTE: The data returned MUST be freed by the caller.
 *
 *   llName = TwList name
 *   length = Pointer to int to place length into.
 *   block = if true then block waiting on an item else return null if queue empty
 *
 *   returns NULL on error or empty queue
 *           else pointer to data
 */
TglWidget *twRemove(TwList *lp, bool block) {
    TglWidget *tw = NULL;
	TwData *h = NULL;
	TwData *t = NULL;

	if (lp == NULL)
		return NULL;

	if (lp->inUse == 1) {
		pthread_mutex_lock(&(lp->llock));

		if (lp->itemCount <= 0 && block == false) {
			pthread_mutex_unlock(&(lp->llock));
			return tw;
		}

		while (lp->itemCount == 0) {
			pthread_cond_wait(&(lp->lcond), &(lp->llock));
		}

		h = lp->head;
		t = h->next;
		tw = (TglWidget *)calloc(1, sizeof(TglWidget));
		tw = h->tw;
		lp->head = t;
		if (lp->itemCount > 0)
			lp->itemCount--;
		else
			lp->itemCount = 0;

		pthread_mutex_unlock(&(lp->llock));
	}

	return tw;
}

/* This function sllDestroy frees up the memory used by the given named list.
 *
 *  sllName = list name
 *
 *  return -1 on error
 *         else 0 on success
 */
int twDestroy(TwList *lp) {

	if (lp == NULL)
		return -1;

	if (lp->inUse == 1) {
		pthread_mutex_lock(&(lp->llock));

		// if (lp->tw != NULL)
		// 	free(lp->tw);

		TwData *ld = lp->head;
		while (ld != NULL) {
			TwData *t = ld->next;
			free(ld);
			ld = t;
		}

		lp->inUse = 0;

		pthread_mutex_unlock(&(lp->llock));
	}

	return 0;
}

/*
 * This function llCount returns the number of item in the named queue.
 *
 *   llName = TwList name
 *
 *   returns number items in queue.
 */
int twCount(TwList *lp) {
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
 *  llName = TwList name
 *
 *  returns number items in queue.
 */
TglWidget *twList(TwList *lp, int *length) {

	if (lp == NULL)
		return NULL;

	if (lp->inUse == 1) {
		pthread_mutex_lock(&(lp->llock));

		// if (lp->list != NULL)
		// 	free(lp->list);

		if (length != NULL)
			*length = lp->itemCount;

		lp->list = (TglWidget *)calloc(lp->itemCount + 1, sizeof(TglWidget *));
		printf("Allocated %d\n", (lp->itemCount + 1) * sizeof(TglWidget *));
		TglWidget *twp = lp->list;

		TwData *ld = lp->head;
		while (ld != NULL) {
			*twp = ld->tw;
			printf("twp %p\n", twp);
			twp++;
			ld = ld->next;
		}

		pthread_mutex_unlock(&(lp->llock));
	}

    return lp->list;
}
#endif
