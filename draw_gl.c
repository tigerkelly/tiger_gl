#include <linux/fb.h>

#include "tiger_gl.h"

extern TglScreen *_ts;
extern TglInfo *_tglInfo;
extern DirtyArea *dirtyArea;
extern int dirtyAreaQue;
extern int dirtyAreaPoolQue;

extern struct fb_var_screeninfo _vinfo;
extern struct fb_fix_screeninfo _finfo;

void _addArea(WidgetType type, int x, int y, int width, int height) {
	DirtyArea *da = NULL;

	ItemType it;
	int r = cqRemove(dirtyAreaPoolQue, &it, CQ_BLOCK);
	if (r != 0) {
		printf("No more dirty area blocks.\n");
		return;
	}

	da = &dirtyArea[it.i];

	da->type = type;
	da->x = x;
	da->y = y;
	da->width = width;
	da->height = height;

	cqAdd(dirtyAreaQue, &it);
}

void _addAreaW(TglWidget *tw) {
	_addArea(tw->widgetType, tw->x, tw->y, tw->width, tw->height);
}

void tglDrawLine(int xs, int ys, int xe, int ye, unsigned int c) {
	UG_DrawLine(xs, ys, xe, ye, c);
	if(_tglInfo->autoUpdate)
		_addArea(TGL_LINE, xs, ys, xe, ye);
}

void tglDrawRect(int x, int y, int width, int height, unsigned int c) {
	if (_ts == NULL)
		return;

	if (_ts->screen == NULL)
		return;
	UG_DrawFrame(x, y, (x + width), (y + height), c);
	if(_tglInfo->autoUpdate)
		_addArea(TGL_RECT, x, y, width, height);
}

void tglDrawRoundRect(int x, int y, int width, int height, int radius, unsigned int c) {
	if (_ts == NULL)
		return;

	if (_ts->screen == NULL)
		return;
	UG_DrawRoundFrame(x, y, (x + width), (y + height), radius, c);
	if(_tglInfo->autoUpdate)
		_addArea(TGL_RECT, x, y, width, height);
}

void tglDrawFillRect(int x, int y, int width, int height, unsigned int c) {
	if (_ts == NULL)
		return;

	if (_ts->screen == NULL)
		return;
	UG_FillFrame(x, y, (x + width), (y + height), c);
	if(_tglInfo->autoUpdate)
		_addArea(TGL_RECT, x, y, width, height);
}

void tglDrawFillRoundRect(int x, int y, int width, int height, int radius, unsigned int c) {
	if (_ts == NULL)
		return;

	if (_ts->screen == NULL)
		return;
	UG_FillRoundFrame(x, y, (x + width), (y + height), radius, c);
	if(_tglInfo->autoUpdate)
		_addArea(TGL_RECT, x, y, width, height);
}

void tglDrawCircle(int x, int y, int radius, unsigned int c) {
	if (_ts == NULL)
		return;

	if (_ts->screen == NULL)
		return;

	UG_DrawCircle(x, y, radius, c);
	if(_tglInfo->autoUpdate)
		_addArea(TGL_CIRCLE, x, y, radius, 0);
}

void tglDrawFillCircle(int x, int y, int radius, unsigned int c) {
	if (_ts == NULL)
		return;

	if (_ts->screen == NULL)
		return;

	UG_FillCircle(x, y, radius, c);
	if(_tglInfo->autoUpdate)
		_addArea(TGL_CIRCLE, x, y, radius, 0);
}

void tglSetBgColor(int bg) {
	UG_SetBackcolor(bg);
}

void tglSetFgColor(int fg) {
	UG_SetForecolor(fg);
}

void tglDrawPutChar(char c, int x, int y, unsigned int fc, unsigned int bc) {
	UG_PutChar(c, x, y, fc, bc);
}

void tglDrawPutString(int x, int y, char *text, unsigned int fc, unsigned int bc) {
	UG_PutString(x, y, text, fc, bc);
}

void tglDrawMeshRect(int x, int y, int width, int height, unsigned int c) {
	UG_DrawMesh(x, y, (x + width), (y + height), c);
	if(_tglInfo->autoUpdate)
		_addArea(TGL_RECT, x, y, width, height);
}

void tglDrawArc( int x, int y, int radius, int sec, unsigned int c) {
	UG_DrawArc(x, y, radius, sec, c);
	if(_tglInfo->autoUpdate)
		_addArea(TGL_ARC, x, y, radius, 0);
}

void tglDrawImage(int x, int y, TGLBITMAP *img, bool transparency) {
	if (img == NULL)
		return;
	
	int width = tglImageGetWidth(img);
	int height = tglImageGetHeight(img);
	int bpp = tglImageGetLine(img) / tglImageGetWidth(img);  // Bytes per pixel
	int length = width * bpp;				// number of bytes to copy

    for (int y2 = 0; y2 < height; y2++, y++) {
		if (transparency == false) {
			unsigned char *p = tglScreenGetScanLine(y) + (x * bpp);
			unsigned char *b = tglImageGetScanLine(img, y2);

			memcpy(p, b, length);
		} else {
			unsigned int *p = (unsigned int *)(tglScreenGetScanLine(y) + (x * bpp));
			unsigned int *b = (unsigned int *)tglImageGetScanLine(img, y2);

			for (int i = 0; i < width; i++, p++, b++) {
				if (*b)
					*p = *b;
			}
		}
    }

	if(_tglInfo->autoUpdate)
		_addArea(TGL_IMAGE, x, y, width, height);
}

// xs and ys are the starting position, width and height are the images width and height 
void tglDrawVideoImage(TglWidget *tw, TGLBITMAP *img) {
	if (_ts == NULL)
		return;

	if (_ts->screen == NULL)
		return;
	
	if (tw == NULL)
		return;

	int screenWidth = tglScreenGetWidth();
	int bytespp = tglScreenGetLine() / screenWidth;
	unsigned char *p = FreeImage_GetBits((FIBITMAP *)img);
	int bpp = tglImageGetLine(img) / tglImageGetWidth(img);

	// printf("Bpp %d\n", bpp);
	// printf("bytespp: %d, %d / %d\n", bytespp,  tglScreenGetLine(), screenWidth);
	// printf("Size: %d\n", FreeImage_GetDIBSize(_ts->screen));

	// printf("xs %d, ys %d, width %d, height %d\n", xs, ys, width, height);

	for (int y = tw->y; y < (tw->y + tw->height); y++) {
		BYTE *bits = tglScreenGetScanLine(y);
		if (tw->x > 0)
			bits += (bytespp * tw->x);
		for (int x = tw->x; x < (tw->x + tw->width); x++) {
			bits[FI_RGBA_RED] = p[FI_RGBA_RED];
			bits[FI_RGBA_GREEN] = p[FI_RGBA_GREEN];
			bits[FI_RGBA_BLUE] = p[FI_RGBA_BLUE];
			if (bpp > 3)
				bits[FI_RGBA_ALPHA] = p[FI_RGBA_ALPHA];
			p += bpp;
			bits += bytespp;
		}
	}

	if(_tglInfo->autoUpdate) {
		_addArea(TGL_IMAGE, tw->x, tw->y, tw->width, tw->height);
	}
}

