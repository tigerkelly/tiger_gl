
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#include "timer.h"

void *_waitThread(void *param);

// This is not a vary accurate timer but it will do for my needs. KW

// int timerTask(unsigned int msDelay, int id, void (*task)(int id)) {
int tglTimerTask(Timer *tt) {

	pthread_t timer;

	// Must allocate args being passed to the thread, they can not be on the stack.
	Timer *tInfo = (Timer *)calloc(1, sizeof(Timer));

	// Copy data to allocated Timer structure.
	*tInfo = *tt;

	// printf("1 tInfo->msDelay %ld, tInfo->id %d\n", tInfo->msDelay, tInfo->id);

	if (pthread_create(&timer, NULL, _waitThread, (void *)tInfo)) {
		printf("Timer wait thread failed.\n");
		return -1;
	}

	return 0;
}

void *_waitThread(void *param) {

	// printf("_waitThread started.\n");

	Timer tt = *(Timer *)param;

	// Need to free what we allocated in the timertask function.
	free(param);

	// printf("2 tt.msDelay %ld, tt.id %d\n", tt.msDelay, tt.id);

	struct timespec t;
	t.tv_sec = (time_t)(tt.msDelay / 1000);
	t.tv_nsec = (long)((tt.msDelay % 1000) * 1000000);

	// printf("sec %ld, nsec %ld\n", t.tv_sec, t.tv_nsec);

	nanosleep(&t, NULL);

	if (tt.task)
		(tt.task)(tt.id);

	return NULL;
}
