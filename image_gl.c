
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <linux/fb.h>
#include <FreeImage.h>

#include "tiger_gl.h"
#include "ugui.h"

TGLBITMAP *tglImageCreate(int x, int y, int width, int height, int bpp) {

	FIBITMAP *img = FreeImage_Allocate(width, height, bpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);
	if (img == NULL) {
		return NULL;
	}

	return (TGLBITMAP *)img;
}

TGLBITMAP *tglImageLoad(char *fileName) {

	if (fileName == NULL)
		return NULL;

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
		return NULL;
	}

	FIBITMAP *img = FreeImage_Load(type, fileName, 0);
	if (img == NULL) {
		return NULL;
	}

	FreeImage_FlipVertical(img);

	// printf("IsTransparent %d, bpp %d\n", FreeImage_IsTransparent(img), FreeImage_GetBPP(img));
	// printf("TransparencyTable %p\n", FreeImage_GetTransparencyTable(img));
	// printf("TransparencyCount %d\n", FreeImage_GetTransparencyCount(img));


	return (TGLBITMAP *)img;
}

void tglImageSetTransparent(TGLBITMAP *img, bool flag) {
	FreeImage_SetTransparent((FIBITMAP *)img, flag);
}

void tglImageSetTransTable(TGLBITMAP *img, BYTE *table, int count) {
	FreeImage_SetTransparencyTable((FIBITMAP *)img, table, count);
}

void tglImageGetBg(TGLBITMAP *img, TGLRGB *rgb) {
	FreeImage_GetBackgroundColor((FIBITMAP *)img, rgb);
}

unsigned myReadProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
	return fread(buffer, size, count, (FILE *)handle);
}

unsigned myWriteProc(void *buffer, unsigned size, unsigned count, fi_handle handle) {
	return fwrite(buffer, size, count, (FILE *)handle);
}

int mySeekProc(fi_handle handle, long offset, int origin) {
	return fseek((FILE *)handle, offset, origin);
}

long myTellProc(fi_handle handle) {
	return ftell((FILE *)handle);
}

TGLBITMAP *tglImageLoadMem(unsigned char *mem, int memLength) {

	FIMEMORY *hmem = FreeImage_OpenMemory(mem, memLength);

	FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(hmem, 0);

	FIBITMAP *img = FreeImage_LoadFromMemory(fif, hmem, 0);

	// printf("width %d height %d Bpp %d\n", 
	// 		FreeImage_GetWidth(img),
	// 		FreeImage_GetHeight(img),
	// 		FreeImage_GetBPP(img));

	FreeImage_CloseMemory(hmem);

	return (TGLBITMAP *)img;
}

int tglImageSave(TGLBITMAP * img, char *fileName) {

	if (img == NULL)
		return -1;

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

	if (FreeImage_Save(type, img, fileName, 0) == 0)
		return -2;

	return 0;
}

int tglImageAppend(FILE *fd, char *img, int imgSize) {
	int r = fwrite(img, 1, imgSize, fd);

	return r;
}

int tglImageDelete(TGLBITMAP *img) {

	if (img == NULL)
		return -1;

	FreeImage_Unload((FIBITMAP *)img);

	return 0;
}

int tglImageSaveRaw(unsigned char *bits, int width, int height, int bpp, char *fileName) {

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

	FIBITMAP *t = FreeImage_ConvertFromRawBits(bits,
			width, height, ((((bpp * width) + 31) / bpp) * 4), bpp,
			FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, false);

	// NOTE: Image is always up side down, even with FreeImage_FlipVertical() ???
	if (FreeImage_Save(type, t, fileName, 0) == 0) {
		printf("Save of screen image failed.\n");
		return -2;
	}

	return 0;
}

TGLBITMAP *tglImageComposite(TGLBITMAP *img, unsigned int fc) {
	RGBQUAD rgb;
	rgb.rgbRed = (fc >> (FI_RGBA_RED * 8)) & 0xff;
	rgb.rgbGreen = (fc >> (FI_RGBA_GREEN * 8)) & 0xff;
	rgb.rgbBlue = (fc >> (FI_RGBA_BLUE * 8)) & 0xff;
	return (TGLBITMAP *)FreeImage_Composite((FIBITMAP *)img, false, &rgb, NULL);
}

TGLBITMAP *tglImageClone(TGLBITMAP *img) {
	return (TGLBITMAP *)FreeImage_Clone((FIBITMAP *)img);
}

BYTE *tglImageGetScanLine(TGLBITMAP *img, int row) {
	if (img == NULL)
		return NULL;

	return FreeImage_GetScanLine((FIBITMAP *)img, row);
}

unsigned char *tglImageGetBits(TGLBITMAP *img) {
	return FreeImage_GetBits(img);
}

unsigned tglImageGetBPP(TGLBITMAP *img) {
	if (img == NULL)
		return 0;

	return FreeImage_GetBPP((FIBITMAP *)img);
}

int tglImageGetType(TGLBITMAP *img) {
	if (img == NULL)
		return -1;

	return FreeImage_GetImageType((FIBITMAP *)img);
}

int tglImageGetColorsUsed(TGLBITMAP *img) {
	if (img == NULL)
		return -1;

	return FreeImage_GetColorsUsed((FIBITMAP *)img);
}

int tglImageGetWidth(TGLBITMAP *img) {
	if (img == NULL)
		return -1;

	return FreeImage_GetWidth((FIBITMAP *)img);
}

int tglImageGetHeight(TGLBITMAP *img) {
	if (img == NULL)
		return -1;

	return FreeImage_GetHeight((FIBITMAP *)img);
}

unsigned tglImageGetPitch(TGLBITMAP *img) {
	if (img == NULL)
		return 0;

	return FreeImage_GetPitch((FIBITMAP *)img);
}

unsigned tglImageGetLine(TGLBITMAP *img) {
	if (img == NULL)
		return 0;

	return FreeImage_GetLine((FIBITMAP *)img);
}

TGLBITMAP *tglImageRotate(TGLBITMAP *img, double angle) {
	if (img == NULL)
		return NULL;

	FIBITMAP *t = FreeImage_Rotate(img, angle, NULL);
	if (t == NULL) {
		return NULL;
	}

	return (TGLBITMAP *)t;
}

int tglImageFlipHorizontal(TGLBITMAP *img) {
	if (img == NULL)
		return -1;

	return FreeImage_FlipHorizontal((FIBITMAP *)img);
}

int tglImageFlipVertical(TGLBITMAP *img) {
	if (img == NULL)
		return -1;

	return FreeImage_FlipVertical((FIBITMAP *)img);
}

unsigned tglImageGetSize(TGLBITMAP *img) {
	if (img == NULL)
		return 0;

	return FreeImage_GetDIBSize((FIBITMAP *)img);
}

TGLBITMAP *tglImageRescale(TGLBITMAP *img, int width, int height, TglFilter filter) {
	if (img == NULL)
		return NULL;

	FIBITMAP *t = FreeImage_Rescale(img, width, height, filter);
	if (t == NULL) {
		return NULL;
	}

	return (TGLBITMAP *)t;
}

TGLBITMAP *tglImageConvertTo32(TGLBITMAP *img) {
	return (TGLBITMAP *)FreeImage_ConvertTo32Bits((FIBITMAP *)img);
}

void tglImagePrintInfo(TGLBITMAP *img, char *title) {
	if (img == NULL)
		return;

	if (title != NULL)
		printf("--- %s ---\n", title);
	printf("Width:  %d\n", FreeImage_GetWidth((FIBITMAP *)img));
	printf("Height: %d\n", FreeImage_GetHeight((FIBITMAP *)img));
	printf("Bpp:    %d\n", FreeImage_GetBPP((FIBITMAP *)img));
	printf("Pitch:  %d\n", FreeImage_GetPitch((FIBITMAP *)img));
	printf("Size:   %d\n", FreeImage_GetDIBSize((FIBITMAP *)img));
	printf("--------------\n");
}
