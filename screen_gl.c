
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <FreeImage.h>

#include "tiger_gl.h"
#include "ugui.h"

TglScreen *_ts = NULL;

extern TglInfo *_tglInfo;
extern DirtyArea *dirtyArea;
extern int dirtyAreaQue;
extern int dirtyAreaPoolQue;

extern void _addArea(WidgetType type, int x, int y, int width, int height);
extern void _addAreaW(TglWidget *tw);

int tglScreenCreate(int x, int y, int width, int height, int bpp) {

	_ts = (TglScreen *)calloc(1, sizeof(TglScreen));
	if (_ts == NULL)
		return -1;

	_ts->x = x;
	_ts->y = y;
	_ts->width = width;
	_ts->height = height;
	_ts->bpp = bpp;

	_ts->screen = FreeImage_Allocate(width, height, bpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
	if (_ts->screen == NULL) {
		free(_ts);
		return -2;
	}

	return 0;
}

int tglScreenLoad(char *fileName) {

	if (fileName == NULL)
		return -1;

	_ts = (TglScreen *)calloc(1, sizeof(TglScreen));

	if (_ts == NULL)
		return -2;

	char *p = rindex(fileName, '.');

	int type = 0;

	if (strcasecmp(p, ".png") == 0)
		type = FIF_PNG;
	else if (strcasecmp(p, ".bmp") == 0)
		type = FIF_BMP;
	else if (strcasecmp(p, ".jpeg") == 0 ||
			strcasecmp(p, ".jpg") == 0 ||
			strcasecmp(p, ".jif") == 0 ||
			strcasecmp(p, ".jpe") == 0)
		type = FIF_JPEG;
	else if (strcasecmp(p, ".gif") == 0)
		type = FIF_GIF;
	else if (strcasecmp(p, ".j2k") == 0 ||
			strcasecmp(p, ".j2c") == 0)
		type = FIF_J2K;
	else if (strcasecmp(p, ".jng") == 0)
		type = FIF_JNG;
	else if (strcasecmp(p, ".ico") == 0)
		type = FIF_ICO;
	else if (strcasecmp(p, ".ico") == 0)
		type = FIF_ICO;
	else {
		printf("Error: Unknown image type.\n");
		free(_ts);
		return -3;
	}

	_ts->screen = FreeImage_Load(type, fileName, 0);
	if (_ts->screen == NULL) {
		free(_ts);
		return -3;
	}

	_ts->width = FreeImage_GetWidth(_ts->screen);
	_ts->height = FreeImage_GetHeight(_ts->screen);

	return 0;
}

int tglScreenSave(char *fileName) {

	char *p = rindex(fileName, '.');

	int type = 0;

	if (strcasecmp(p, ".png") == 0)
		type = FIF_PNG;
	else if (strcasecmp(p, ".bmp") == 0)
		type = FIF_BMP;
	else if (strcasecmp(p, ".jpeg") == 0 ||
			strcasecmp(p, ".jpg") == 0 ||
			strcasecmp(p, ".jif") == 0 ||
			strcasecmp(p, ".jpe") == 0)
		type = FIF_JPEG;
	else if (strcasecmp(p, ".gif") == 0)
		type = FIF_GIF;
	else if (strcasecmp(p, ".j2k") == 0 ||
			strcasecmp(p, ".j2c") == 0)
		type = FIF_J2K;
	else if (strcasecmp(p, ".jng") == 0)
		type = FIF_JNG;
	else if (strcasecmp(p, ".ico") == 0)
		type = FIF_ICO;
	else if (strcasecmp(p, ".ico") == 0)
		type = FIF_ICO;
	else {
		printf("Error: Unknown image type.\n");
		return -1;
	}

	FIBITMAP *t = FreeImage_ConvertFromRawBits(_tglInfo->fbp,
			_tglInfo->width, _tglInfo->height, ((((32 * _tglInfo->width) + 31) / 32) * 4), 32,
			FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, false);

	if (FreeImage_Save(type, t, fileName, 0) == 0) {
		printf("Save of screen image failed.\n");
		return -2;
	}

	return 0;
}

int tglScreenDelete() {

	if (_ts == NULL)
		return -1;

	if (_ts->screen != NULL) {
		FreeImage_Unload(_ts->screen);
	}

	free(_ts);

	return 0;
}

TGLBITMAP *tglScreenGetBitmap() {
	if (_ts == NULL)
		return NULL;
	if (_ts->screen == NULL)
		return NULL;

	return (TGLBITMAP *)_ts->screen;
}

BYTE *tglScreenGetPixels() {
	if (_ts == NULL)
		return NULL;

	if (_ts->screen == NULL)
		return NULL;

	return FreeImage_GetBits(_ts->screen);
}

BYTE *tglScreenGetScanLine(int row) {
	if (_ts == NULL)
		return NULL;

	if (_ts->screen == NULL)
		return NULL;

	return FreeImage_GetScanLine(_ts->screen, row);
}

int tglScreenGetType() {
	if (_ts == NULL)
		return -1;

	if (_ts->screen == NULL)
		return -2;

	return FreeImage_GetImageType(_ts->screen);
}

int tglScreenGetColorsUsed() {
	if (_ts == NULL)
		return -1;

	if (_ts->screen == NULL)
		return -2;

	return FreeImage_GetColorsUsed(_ts->screen);
}

int tglScreenGetBpp() {
	if (_ts == NULL)
		return -1;

	if (_ts->screen == NULL)
		return -2;

	return FreeImage_GetBPP(_ts->screen);
}

int tglScreenGetWidth() {
	if (_ts == NULL)
		return -1;

	if (_ts->screen == NULL)
		return -2;

	return FreeImage_GetWidth(_ts->screen);
}

int tglScreenGetHeight() {
	if (_ts == NULL)
		return -1;

	if (_ts->screen == NULL)
		return -2;

	return FreeImage_GetHeight(_ts->screen);
}

unsigned tglScreenGetPitch() {
	if (_ts == NULL)
		return 0;

	if (_ts->screen == NULL)
		return 0;

	return FreeImage_GetPitch(_ts->screen);
}

unsigned tglScreenGetLine() {
	if (_ts == NULL)
		return 0;

	if (_ts->screen == NULL)
		return 0;

	return FreeImage_GetLine(_ts->screen);
}

int tglScreenRotate(double angle) {
	if (_ts == NULL)
		return -1;

	if (_ts->screen == NULL)
		return -2;

	TglScreen *t = (TglScreen *)calloc(1, sizeof(TglScreen));
	if (t == NULL)
		return -3;

	FIBITMAP *img = FreeImage_Rotate(_ts->screen, angle, NULL);
	if (img == NULL) {
		return -4;
	}

	FIBITMAP *f = _ts->screen;
	_ts->screen = img;
	if (f)
		free(f);


	return 0;
}

int tglScreenFlipHorizontal() {
	if (_ts == NULL)
		return -1;

	if (_ts->screen == NULL)
		return -2;

	return FreeImage_FlipHorizontal(_ts->screen);
}

int tglScreenFlipVertical() {
	if (_ts == NULL)
		return -1;

	if (_ts->screen == NULL)
		return -2;

	return FreeImage_FlipVertical(_ts->screen);
}

unsigned tglScreenGetSize() {
	if (_ts == NULL)
		return 0;

	if (_ts->screen == NULL)
		return 0;

	return FreeImage_GetDIBSize(_ts->screen);
}

int tglScreenRescale(int width, int height, TglFilter filter) {
	if (_ts == NULL)
		return -1;

	if (_ts->screen == NULL)
		return -2;

	TglScreen *t = (TglScreen *)calloc(1, sizeof(TglScreen));
	if (t == NULL)
		return -3;

	t->x = _ts->x;
	t->y = _ts->y;
	t->width = width;
	t->height = height;

	t->screen = FreeImage_Rescale(_ts->screen, width, height, filter);
	if (t->screen == NULL) {
		free(t);
		return -4;
	}

	TglScreen *f = _ts;

	_ts = t;
	if (f->screen)
		free(f->screen);
	free(f);

	return 0;
}

void tglScreenPrintInfo(char *title) {
	if (_ts == NULL)
		return;

	if (_ts->screen == NULL)
		return;

	if (title != NULL)
		printf("--- %s ---\n", title);
	printf("Width:  %d\n", FreeImage_GetWidth(_ts->screen));
	printf("Height: %d\n", FreeImage_GetHeight(_ts->screen));
	printf("Bpp:    %d\n", FreeImage_GetBPP(_ts->screen));
	printf("Pitch:  %d\n", FreeImage_GetPitch(_ts->screen));
	printf("Size:   %d\n", FreeImage_GetDIBSize(_ts->screen));
	printf("--------------\n");
}

void tglScreenPutPixel(short x, short y, unsigned int c) {
	if (_ts == NULL)
		return;

	if (_ts->screen == NULL)
		return;

	int screenWidth = tglScreenGetWidth();
	int screenHeight = tglScreenGetHeight();

	if (x > screenWidth)
		return;

	if (y > screenHeight)
		return;

	// printf("x %d, y %d c 0x%x\n", x, y, c);

	int bytespp = tglScreenGetLine() / screenWidth;
	BYTE *bits = tglScreenGetScanLine(y);
	bits += (bytespp * x);
	bits[FI_RGBA_RED] = (c >> (FI_RGBA_RED * 8)) & 0xff;
	bits[FI_RGBA_GREEN] = (c >> (FI_RGBA_GREEN * 8)) & 0xff;
	bits[FI_RGBA_BLUE] = (c >> (FI_RGBA_BLUE * 8)) & 0xff;
}

void tglScreenPutChar(char c, int x, int y, unsigned int fc, unsigned int bc) {
	UG_PutChar(c, x, y, fc, bc);
}

void tglScreenPutString(int x, int y, char *text, unsigned int fc, unsigned int bc) {
	UG_PutString(x, y, text, fc, bc);
}

void tglScreenFill(unsigned int color) {
	UG_FillScreen(color);
}
void tglScreenSetFont(char *fontName) {
	UG_FontSelectByName(fontName);
}

void tglScreenUpdate() {
	tglFbUpdate();
	// UG_Update();
}
