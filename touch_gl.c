
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/input.h>

#include "touch.h"
#include "tiger_gl.h"

int _stopTouchEvent = 0;
int noPressureFlag = 0;
int rotateTouch = 0;
int touchFd = -1;
// char *deviceName = NULL;
double  sWidth = 0.0;
double  sHeight = 0.0;
double  tWidth = 0.0;
double  tHeight = 0.0;

pthread_t touchEvent;
pthread_t touchEvent;

void *_touchThread(void *param);
int getTouchSample(int *rawX, int *rawY, int *rawPressure);
void getTouchScreenDetails(int *screenXmin,int *screenXmax,int *screenYmin,int *screenYmax);

extern TglInfo *_tglInfo;
extern void tglWidgetEvent(int x, int y, int p, int t);

int tglTouchInit(char *device, 
		int screenWidth, int screenHeight, int touchWidth, int touchHeight,
		int pressureFlag, int rotate) {

	_tglInfo->touchDevice = strdup(device);
	
	sWidth = (double)screenWidth;
	sHeight = (double)screenHeight;
	tWidth = (double)touchWidth;
	tHeight = (double)touchHeight;

	noPressureFlag = pressureFlag;
	rotateTouch = rotate;

	if (screenWidth <= 0 || screenHeight <= 0) {
		printf("Screen resolution is invalid.\n");
		return -1;
	}

	if (touchWidth <= 0 || touchHeight <= 0) {
		printf("Touch resolution is invalid.\n");
		return -1;
	}

	if (pthread_create(&touchEvent, NULL, _touchThread, NULL)) {
        printf("Error: Can not create thread touch event.\n");
        return 1;
    }

	return 0;
}

void tglTouchStopEvent() {
	if (_stopTouchEvent)
		return;
	_stopTouchEvent = 1;
	if (touchFd != -1) {
		close(touchFd);
		touchFd = -1;
	}
}

void tglTouchGetEvent() {
 
	int rawX;
	int rawY;
	int rawPressure;

	int r = 0;
	if ((r = getTouchSample(&rawX, &rawY, &rawPressure)) > 0) {
		// printf("rawX %d, rawY %d, rawPressure %d, r %d\n", rawX, rawY, rawPressure, r);
		// printf("rotate %d, tWidth %0.1f, tHeight %0.1f\n", rotateTouch, tWidth, tHeight);

		// Have to adjust the touch point to match the screen resolution.
		// Screen resolution is 1024x600, on 5 inch but the touch resolution is only 800x480
		// Screen resolution is 1280x720, on 7 inch but the touch resolution is only 1024x600
		// So we scale up the rawX and rawY to match the 1024x600 of the resolution.
		double xScale = (sWidth / tWidth);
		double yScale = (sHeight / tHeight);

		if (rotateTouch == 1) {				// rotate touch by 90 degrees.
			rawX = tWidth - rawX;
			rawY = tHeight - rawY;
		} else if (rotateTouch == 2) {		// rotate touch by 180 degrees.
		} else if (rotateTouch == 3) {		// rotate touch by 270 degrees.
		}

		if (sWidth == tWidth && sHeight == tHeight) {		// in case they are the same.
			tglWidgetEvent(rawX, rawY, rawPressure, r);
		} else if (sWidth > tWidth && sHeight > tHeight) {
			int xWhole = (int)xScale;				// get the whole number part.
			int yWhole = (int)yScale;
			xScale = xScale - ((long)xScale);		// get the fractional part of scale.
			yScale = yScale - ((long)yScale);
			// printf("xScale: %f, yScale: %f\n", xScale, yScale);
			// printf("Starting rawX %d, rawY %d\n", rawX, rawY);
			rawX += (int)((double)(rawX / xWhole) * xScale);
			rawY += (int)((double)(rawY / yWhole) * yScale);

			tglWidgetEvent(rawX, rawY, rawPressure, r);
		} else {
			// if screen resolution is less than touch resolution.
			xScale = xScale - ((long)xScale);		// get the fractional part of scale.
			yScale = yScale - ((long)yScale);
			// printf("xScale: %f, yScale: %f\n", xScale, yScale);
			// printf("Starting rawX %d, rawY %d\n", rawX, rawY);
			rawX -= (int)((double)rawX * xScale);
			rawY -= (int)((double)rawY * yScale);

			tglWidgetEvent(rawX, rawY, rawPressure, r);
		}
	}
}

void *_touchThread(void *param) {

	touchFd = open(_tglInfo->touchDevice, O_RDONLY);
	if (touchFd == -1) {
		printf("Can not open touch device %s\n", _tglInfo->touchDevice);
		perror("");
		return NULL;
	}

	while (_stopTouchEvent == 0) {
		tglTouchGetEvent();
	}

	return NULL;
}

#if(0)
/*
   A simple program that demonstrates how to program for a
   touch screen. Specifically on the Raspberry Pi.
   This prgram can be used for most Linux based systems.
   For more details: ozzmaker.com

   Copyright (C) 2013  Mark Williams

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
   MA 02111-1307, USA
 */
#define KWHT  "\x1B[32m"
#define KYEL  "\x1B[33m"


void getTouchScreenDetails(int *screenXmin,int *screenXmax,int *screenYmin,int *screenYmax) {
	unsigned long bit[EV_MAX][NBITS(KEY_MAX)];
	char name[256] = "Unknown";
	int abs[6] = {0};

	ioctl(touchFd, EVIOCGNAME(sizeof(name)), name);
	printf("Input device name: \"%s\"\n", name);

	memset(bit, 0, sizeof(bit));
	ioctl(touchFd, EVIOCGBIT(0, EV_MAX), bit[0]);
	printf("Supported events:\n");

	int i,j,k;
	for (i = 0; i < EV_MAX; i++) {
		if (test_bit(i, bit[0])) {
			printf("  Event type %d (%s)\n", i, events[i] ? events[i] : "?");
			if (!i) continue;
			ioctl(touchFd, EVIOCGBIT(i, KEY_MAX), bit[i]);
			for (j = 0; j < KEY_MAX; j++){
				if (test_bit(j, bit[i])) {
					printf("    Event code %d (%s)\n", j, names[i] ? (names[i][j] ? names[i][j] : "?") : "?");
					if (i == EV_ABS) {
						ioctl(touchFd, EVIOCGABS(j), abs);
						for (k = 0; k < 5; k++)
							if ((k < 3) || abs[k]){
								printf("     %s %6d\n", absval[k], abs[k]);
								if (j == 0){
									if (strcmp(absval[k], "Min  ") == 0)
										*screenXmin =  abs[k];
									if (strcmp(absval[k], "Max  ") == 0)
										*screenXmax =  abs[k];
								}
								if (j == 1){
									if (strcmp(absval[k], "Min  ") == 0)
										*screenYmin =  abs[k];
									if (strcmp(absval[k], "Max  ") == 0)
										*screenYmax =  abs[k];
								}
							}
					}

				}
			}
		}
	}
}
#endif

// Returns bit pattern of events.
int getTouchSample(int *rawX, int *rawY, int *rawPressure) {
	int ret = 0;
	size_t rb;					// how many bytes were read
	struct input_event ev;
	static int oldX = 0, oldY = 0, oldPressure = 0;

	*rawX = 0;
	*rawY = 0;
	*rawPressure = 0;

	int startTouch = (TOUCH_START | TOUCH_X | TOUCH_Y | TOUCH_PRESSURE);

	while(_stopTouchEvent == 0) {
		rb = read(touchFd, &ev, sizeof(struct input_event));
		// printf("Here 2: %d ev.code %d, ev.value %d ev.type %d\n", rb, ev.code, ev.value, ev.type);

		if (rb > 0) {
			if (ev.type ==  EV_SYN) {
			} else if (ev.type == EV_KEY && ev.code == 330 && ev.value == 1) {	// start of touch
				ret |= TOUCH_START;
			} else if (ev.type == EV_KEY && ev.code == 330 && ev.value == 0) {	// end of touch
				ret = TOUCH_STOP;
				*rawX = oldX;
				*rawY = oldY;
				*rawPressure = oldPressure;
				oldX = oldY = oldPressure = 0;
			} else if (ev.type == EV_ABS && ev.code == 0   && ev.value > 0) {	// get touch X position
				oldX = *rawX = ev.value;
				ret |= TOUCH_X;
			} else if (ev.type == EV_ABS && ev.code == 1   && ev.value > 0) {	// get touch Y position
				oldY = *rawY = ev.value;
				ret |= TOUCH_Y;
			} else if (ev.type == EV_ABS && ev.code == 24  && ev.value > 0) {	// get touch pressure
				oldPressure = *rawPressure = ev.value;
				ret |= TOUCH_PRESSURE;
			}

			if (ret == startTouch || ret == TOUCH_STOP)
				break;
		}
	}

	return ret;
}

