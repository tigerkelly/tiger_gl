
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <pthread.h>
#include <stdatomic.h>
#include <FreeImage.h>

#include "tiger_gl.h"
#include "ugui.h"
#include "strutils.h"
#include "ini.h"

#define DIRTY_AREA_QUEUE_SIZE	256
#define TW_QUEUE_SIZE			64

extern TglScreen *_ts;		// initalized by tglScreenCreate()
extern void _paintButton(TglWidget *tw, bool flag);
extern void _paintSpinner(TglWidget *tw, bool flag);
extern void _addAreaW(TglWidget *tw);

struct fb_var_screeninfo _vinfo;
struct fb_var_screeninfo _orig;
struct fb_fix_screeninfo _finfo;

int _currId = 0;
TglInfo *_tglInfo = NULL;
SavedArea _savedArea;
int _closedFlag = 0;

TwList *_twWidgets = 0;

DirtyArea *dirtyArea = NULL;
int dirtyAreaQue = 0;
int dirtyAreaPoolQue = 0;

int twQue = 0;

void *_updateThread(void *param);
pthread_t _updThread;

void *_updateButton(void *param);
pthread_t _updButton;

IniFile *ini = NULL;

UG_GUI _gui;

int _stopUpdateThread = 0;

int tglInit(char *device, uint16_t width, uint16_t height) {
	UG_Init(&_gui, tglScreenPutPixel, width, height);

	// llInit(2);

	ini = iniCreate("tiger_gl.ini");

	_twWidgets = twCreate();

	FreeImage_Initialise(0);

	printf("FreeImage %s %s\n", FreeImage_GetVersion(), FreeImage_GetCopyrightMessage());
	printf("%s", UG_Version());

	return tglFbOpen(device);
}

int tglFbOpen(char *device) {

	_tglInfo = (TglInfo *)calloc(1, sizeof(TglInfo));

	_tglInfo->deviceName = strdup(device);
	_tglInfo->autoUpdate = TGL_AUTOUPDATE;

	_tglInfo->fbfd = open(_tglInfo->deviceName, O_RDWR);
	if (_tglInfo->fbfd == -1) {
		printf("Error: cannot open framebuffer device. %s\n", _tglInfo->deviceName);
		free(_tglInfo);
		return -5;
	}

	// Open tty so I can hide cursor.
	_tglInfo->tty1 = open("/dev/tty1", O_RDWR);
	if (_tglInfo->fbfd == -1) {
		printf("Error: cannot open console device. %s\n", _tglInfo->deviceName);
		close(_tglInfo->fbfd);
		free(_tglInfo);
		return -6;
	}

	// Get fixed screen information
	if (ioctl(_tglInfo->fbfd, FBIOGET_FSCREENINFO, &_finfo)) {
		printf("Error reading fixed information.\n");
		close(_tglInfo->fbfd);
		close(_tglInfo->tty1);
		free(_tglInfo);
		return -7;
	}

	// Get variable screen information
	if (ioctl(_tglInfo->fbfd, FBIOGET_VSCREENINFO, &_vinfo)) {
		printf("Error reading variable information.\n");
		close(_tglInfo->fbfd);
		close(_tglInfo->tty1);
		free(_tglInfo);
		return -8;
	}

	_tglInfo->width = _vinfo.xres;
	_tglInfo->height = _vinfo.yres;
	_tglInfo->bpp = _vinfo.bits_per_pixel;
	_tglInfo->byteCnt = (_vinfo.bits_per_pixel / 8);
	_tglInfo->origScreen = calloc(1, _vinfo.xres * _vinfo.yres * (_tglInfo->bpp / 8));

	memcpy(&_orig, &_vinfo, sizeof(struct fb_var_screeninfo));

	// map framebuffer to user space
	_tglInfo->fbSize = _finfo.smem_len;

	_tglInfo->fbp = (unsigned char *)mmap(0, _tglInfo->fbSize, PROT_READ | PROT_WRITE, MAP_SHARED, _tglInfo->fbfd, 0);

	if ((int) _tglInfo->fbp == -1) {
		printf("Failed to mmap.\n");
		close(_tglInfo->fbfd);
		close(_tglInfo->tty1);
		free(_tglInfo->origScreen);
		free(_tglInfo);
		return -9;
	}

	// save image of screen so that we can restore it at shutdown.
	memcpy(_tglInfo->origScreen, _tglInfo->fbp, _tglInfo->fbSize);

	// turn off console cursor.
	write(_tglInfo->tty1, "\033[?25l", 6);

	if (_tglInfo->autoUpdate) {
		int r = 0;
		if ((r = cqInit(3))) {
			printf("Cqueue init failed. %d\n", r);
			return -10;
		}

		twQue = cqCreate("twQue", TW_QUEUE_SIZE + 1);	// extra slot for head/tail
		if (twQue == -1) {
			printf("Cqueue create failed. %d\n", r);
			return -12;
		}
		// ----------------------

		dirtyArea = (DirtyArea *)calloc(DIRTY_AREA_QUEUE_SIZE, sizeof(DirtyArea));
		if (dirtyArea == NULL) {
			printf("Can no allocate dirty area queue space.\n");
			return -11;
		}

		dirtyAreaPoolQue = cqCreate("poolQue", DIRTY_AREA_QUEUE_SIZE + 1);	// extra slot for head/tail
		if (dirtyAreaPoolQue == -1) {
			printf("Cqueue create failed. %d\n", r);
			return -12;
		}

		ItemType it;
		for (int i = 0; i < DIRTY_AREA_QUEUE_SIZE; i++) {
			it.i = i;
			cqAdd(dirtyAreaPoolQue, &it);
		}

		dirtyAreaQue = cqCreate("areaQue", DIRTY_AREA_QUEUE_SIZE + 1);	// extra slot for head/tail
		if (dirtyAreaQue == -1) {
			printf("Cqueue create failed. %d\n", r);
			return -12;
		}

		if (pthread_create(&_updThread, NULL, _updateThread, NULL)) {
			printf("Error: Can not create thread updateThread\n");
			return -14;
		}

		if (pthread_create(&_updButton, NULL, _updateButton, NULL)) {
			printf("Error: Can not create thread updateButton\n");
			return -14;
		}
	}

	return 0;
}

void tglSetAutoUpdate(int flag) {
	_tglInfo->autoUpdate = flag;
}

void tglFbPrintInfo() {

	printf("Visable: %dx%d, Offset: %dx%d, Virtual: %dx%d, Bpp: %d, Bytes: %ld, ScanLineLength: %d\n",
			_vinfo.xres, _vinfo.yres,
			_vinfo.xoffset, _vinfo.yoffset,
			_vinfo.xres_virtual, _vinfo.yres_virtual,
			_vinfo.bits_per_pixel, (long) _finfo.smem_len, _finfo.line_length);
}

void tglFbClose() {

	if (_closedFlag)
		return;

	_closedFlag = 1;

	if (_tglInfo->deviceName)
		free(_tglInfo->deviceName);

	// show orignal screen.
	memcpy(_tglInfo->fbp, _tglInfo->origScreen, _tglInfo->fbSize);

	if (_tglInfo->origScreen)
		free(_tglInfo->origScreen);

	munmap(_tglInfo->fbp, _tglInfo->fbSize);

	if (ioctl(_tglInfo->fbfd, FBIOPUT_VSCREENINFO, &_orig)) {
		printf("Error re-setting variable information.\n");
	}

	// Make cursor visible.
	write(_tglInfo->tty1, "\033[?25h", 6);

	if (_tglInfo->fbfd >= 0) {
		close(_tglInfo->fbfd);
		_tglInfo->fbfd = -1;
	}

	if (_tglInfo->tty1 >= 0)
		close(_tglInfo->tty1);

	_stopUpdateThread = 1;

}

unsigned char *tglFbGetFbp() {
	return _tglInfo->fbp;
}

int tglFbGetWidth() {
	return _tglInfo->width;
}

int tglFbGetHeight() {
	return _tglInfo->height;
}

int tglFbGetBpp() {
	return _tglInfo->bpp;
}

void tglFbUpdate() {

	int bytespp = tglScreenGetLine() / tglScreenGetWidth();
	int len = tglScreenGetWidth() * bytespp;
	int h = tglScreenGetHeight();

	unsigned char *p = _tglInfo->fbp;
	for(unsigned y = 0; y < h; y++) {
		BYTE *bits = tglScreenGetScanLine(y);
		memcpy(p, bits, len);

		p += len;
	}
}

void tglFbUpdateArea(uint8_t *buf, uint16_t bx, uint16_t by, uint16_t bw, uint16_t bh) {

	if (bw == 0 && bh == 0)
		return;

	int Bpp = (_vinfo.bits_per_pixel / 8);	// Bytes per pixel
	int length = bw * Bpp;					// number of bytes to copy

	for (int y = by; y < (by + bh); y++) {
		int loc1 = (bx * Bpp) + (y * _finfo.line_length);

		unsigned char *p = (_tglInfo->fbp + loc1);
		unsigned char *b = (buf + loc1);

		memcpy(p, b, length);
	}
}

void tglFbSaveArea(uint16_t bx, uint16_t by, uint16_t bw, uint16_t bh) {

	int Bpp = (_vinfo.bits_per_pixel / 8);	// Bytes per pixel
	int length = bw * Bpp;					// number of bytes to copy

	if (_savedArea.area != NULL)
		free(_savedArea.area);

	_savedArea.x = bx;
	_savedArea.y = by;
	_savedArea.width = bw;
	_savedArea.height = bh;

	_savedArea.area = (uint8_t *)calloc(1, (length * bh));

	int row = 0;
	for (int y = by; y < (by + bh); y++) {
		int loc1 = (bx * Bpp) + (y * _finfo.line_length);

		uint8_t *p = (_tglInfo->fbp + loc1);
		uint8_t *b = (_savedArea.area + row);

		memcpy(b, p, length);

		row += length;
	}
}

void tglFbRestoreArea() {

	if (_savedArea.area == NULL)
		return;

	int Bpp = (_vinfo.bits_per_pixel / 8);	// Bytes per pixel
	int length = _savedArea.width * Bpp;		// number of bytes to copy

	int row = 0;
	for (int y = _savedArea.y; y < (_savedArea.y + _savedArea.height); y++) {
		int loc1 = (_savedArea.x * Bpp) + (y * _finfo.line_length);

		uint8_t *p = (_tglInfo->fbp + loc1);
		uint8_t *b = (_savedArea.area + row);

		memcpy(p, b, length);

		row += length;
	}
	free(_savedArea.area);
	_savedArea.area = NULL;
}

void *_updateThread(void *param) {

	ItemType it;
	while(_stopUpdateThread == 0) {
		// When area becomes dirty it is placed on quque.
		int r = cqRemove(dirtyAreaQue, &it, CQ_BLOCK);
		if (r != 0) {
			printf("Remove from dirty area queue failed. %d\n", r);
			// leave thread.
			break;
		}

		DirtyArea *da = &dirtyArea[it.i];

		if (_ts != NULL && _ts->screen != NULL && _stopUpdateThread == 0)
			tglFbUpdateArea(tglImageGetScanLine(_ts->screen, 0), da->x, da->y, da->width, da->height);

		cqAdd(dirtyAreaPoolQue, &it);	// put it back onto free queue.
	}

	return NULL;
}

void *_updateButton(void *param) {

	ItemType it;
	while(_stopUpdateThread == 0) {
		// Update button queue.
		int r = cqRemove(twQue, &it, CQ_BLOCK);
		if (r != 0) {
			printf("Remove from twQue queue failed. %d\n", r);
			// leave thread.
			break;
		}

		struct timespec t = {0, 200000000L };

		nanosleep(&t, NULL);

		TglWidget *tw = (TglWidget *)it.p;

		if (tw->widgetType == WIDGET_BUTTON)
			_paintButton(tw, false);
		else if (tw->widgetType == WIDGET_SPINNER)
			_paintSpinner(tw, false);

		_addAreaW(tw);
	}

	return NULL;
}

void tglFindTouchDevice(char *deviceName) {

	char *dev = NULL;
	char device[128];

	device[0] = '\0';
	
	FILE *in = popen("lsinput", "r");
	if (in) {
		char line[256];

		while(fgets(line, sizeof(line), in) != NULL) {
			trim(line);
			if (strncmp(line, "/dev/", 5) == 0)
				strcpy(device, line);
			else {
				/* Elecrow 5 inch=QDtech MPI5001
				 * Elecrow 7 inch=QDtech MPI7002
				 * MSD-12301=TSTP MTouch, A 12.3" touch screen from Newsoul.
				 */
				if (strncmp(line, "name", 4) == 0) {
					if (strstr(line, "QDtech MPI5001") != NULL) {
						dev = device;
						break;
					} else if (strstr(line, "QDtech MPI7002") != NULL) {
						dev = device;
						break;
					} else if (strstr(line, "TSTP MTouch") != NULL) {
						dev = device;
						break;
					}
				}
			}
		}
		pclose(in);
	}

	if (dev)
		strcpy(deviceName, dev);
}
