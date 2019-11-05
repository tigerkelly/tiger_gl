
#ifndef _TIMER_H_
#define _TIMER_H_

typedef void (*Task)(int id);

typedef struct _timer_ {
	time_t msDelay;
	int id;
	Task task;
} Timer;

int tglTimerTask(Timer *tt);

#endif
