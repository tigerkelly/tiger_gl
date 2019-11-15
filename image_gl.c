
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <linux/fb.h>
#include <FreeImage.h>

#include "tiger_gl.h"
#include "ugui.h"

extern UG_GUI _gui;

TGLBITMAP *tglImageCreate(uint16_t width, uint16_t height, uint16_t bpp) {

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
	else if (strcasecmp(p, ".xbm") == 0)
		type = FIF_XBM;
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

TGLBITMAP *tglImageLoadMem(unsigned char *mem, int memLength, char *type) {

	FIMEMORY *hmem = FreeImage_OpenMemory(mem, memLength);

	FREE_IMAGE_FORMAT fif = FIF_PNG;
	if (type == NULL)
		fif = FreeImage_GetFileTypeFromMemory(hmem, 0);
	else {
		if (strcasecmp(type, "png") == 0)
			fif = FIF_PNG;
		else if (strcasecmp(type, "bmp") == 0)
			fif = FIF_BMP;
		else if (strcasecmp(type, "xbm") == 0)
			fif = FIF_XBM;
		else if (strcasecmp(type, "jpeg") == 0 ||
				strcasecmp(type, "jpg") == 0 ||
				strcasecmp(type, "jif") == 0 ||
				strcasecmp(type, "jpe") == 0)
			fif = FIF_JPEG;
		else if (strcasecmp(type, "gif") == 0)
			fif = FIF_GIF;
		else if (strcasecmp(type, "j2k") == 0 ||
				strcasecmp(type, "j2c") == 0)
			fif = FIF_J2K;
		else if (strcasecmp(type, "jng") == 0)
			fif = FIF_JNG;
		else if (strcasecmp(type, "ico") == 0)
			fif = FIF_ICO;
		else if (strcasecmp(type, "ico") == 0)
			fif = FIF_ICO;
		else {
			printf("Error: Unknown image type.\n");
			return NULL;
		}
	}

	// printf("FIF + %d\n", fif);

	FIBITMAP *img = FreeImage_LoadFromMemory(fif, hmem, 0);

	printf("width %d height %d Bpp %d\n", 
			FreeImage_GetWidth(img),
			FreeImage_GetHeight(img),
			FreeImage_GetBPP(img));

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

int tglImageSaveRaw(uint8_t *bits, uint16_t width, uint16_t height, uint16_t bpp, char *fileName) {

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

TGLBITMAP *tglImageComposite(TGLBITMAP *img, uint32_t fc) {
	RGBQUAD rgb;
	rgb.rgbRed = (fc >> (FI_RGBA_RED * 8)) & 0xff;
	rgb.rgbGreen = (fc >> (FI_RGBA_GREEN * 8)) & 0xff;
	rgb.rgbBlue = (fc >> (FI_RGBA_BLUE * 8)) & 0xff;
	return (TGLBITMAP *)FreeImage_Composite((FIBITMAP *)img, false, &rgb, NULL);
}

TGLBITMAP *tglImageClone(TGLBITMAP *img) {
	return (TGLBITMAP *)FreeImage_Clone((FIBITMAP *)img);
}

BYTE *tglImageGetScanLine(TGLBITMAP *img, uint16_t row) {
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

TGLBITMAP *tglImageRescale(TGLBITMAP *img, uint16_t width, uint16_t height, TglFilter filter) {
	if (img == NULL)
		return NULL;

	FIBITMAP *t = FreeImage_Rescale(img, width, height, filter);
	if (t == NULL) {
		return NULL;
	}

	return (TGLBITMAP *)t;
}

void tglImagePutPixel(TGLBITMAP *img, short x, short y, uint32_t c) {
	BYTE *row = tglImageGetScanLine(img, y);
	row += x;

	row[FI_RGBA_RED] = (c >> (FI_RGBA_RED * 8)) & 0xff;
	row[FI_RGBA_GREEN] = (c >> (FI_RGBA_GREEN * 8)) & 0xff;
	row[FI_RGBA_BLUE] = (c >> (FI_RGBA_BLUE * 8)) & 0xff;
}

void tglImageFill(TGLBITMAP *img, uint32_t c) {

	int width = tglImageGetWidth(img);
	int height = tglImageGetHeight(img);
	for (int y = 0; y < height; y++) {
		BYTE *row = tglImageGetScanLine(img, y);

		for (int x = 0; x < width; x++) {
			row[FI_RGBA_RED] = (c >> (FI_RGBA_RED * 8)) & 0xff;
			row[FI_RGBA_GREEN] = (c >> (FI_RGBA_GREEN * 8)) & 0xff;
			row[FI_RGBA_BLUE] = (c >> (FI_RGBA_BLUE * 8)) & 0xff;

			row += 4;
		}
	}
}

void tglImageDrawLine(TGLBITMAP *img, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t c) {
	short n, dx, dy, sgndx, sgndy, dxabs, dyabs, x, y, drawx, drawy;

	dx = x2 - x1;
    dy = y2 - y1;
    dxabs = (dx>0)?dx:-dx;
    dyabs = (dy>0)?dy:-dy;
    sgndx = (dx>0)?1:-1;
    sgndy = (dy>0)?1:-1;
    x = dyabs >> 1;
    y = dxabs >> 1;
    drawx = x1;
    drawy = y1;

    tglImagePutPixel(img, drawx, drawy, c);

    if( dxabs >= dyabs ) {
        for( n=0; n<dxabs; n++ ) {
            y += dyabs;
            if( y >= dxabs ) {
                y -= dxabs;
                drawy += sgndy;
            }
            drawx += sgndx;
            tglImagePutPixel(img, drawx, drawy, c);
        }
    } else {
        for( n=0; n<dyabs; n++ ) {
            x += dxabs;
            if( x >= dyabs ) {
                x -= dyabs;
                drawx += sgndx;
            }
            drawy += sgndy;
            tglImagePutPixel(img, drawx, drawy, c);
        }
    }
}

void tglImagePutChar(TGLBITMAP *img, uint16_t x, uint16_t y, char chr, uint32_t fc, uint32_t bc, TGL_FONT *font, bool transparency) {

	short i,j,xo,yo,bn,actual_char_width;
    char b,bt;
    uint32_t index;
    uint32_t color;

    bt = (uint8_t)chr;

    switch ( bt ) {
        case 0xF6: bt = 0x94; break; // ö
        case 0xD6: bt = 0x99; break; // Ö
        case 0xFC: bt = 0x81; break; // ü
        case 0xDC: bt = 0x9A; break; // Ü
        case 0xE4: bt = 0x84; break; // ä
        case 0xC4: bt = 0x8E; break; // Ä
        case 0xB5: bt = 0xE6; break; // µ
        case 0xB0: bt = 0xF8; break; // °
    }

    if (bt < font->start_char || bt > font->end_char)
        return;

    yo = y;
    bn = font->char_width;
    if ( !bn )
        return;
    bn >>= 3;
    if ( font->char_width % 8 )
        bn++;
    actual_char_width = (font->widths ? font->widths[bt - font->start_char] : font->char_width);

	index = (bt - font->start_char)* font->char_height * font->char_width;
	for( j = 0; j < font->char_height; j++ ) {
		xo = x;
		for( i = 0; i < actual_char_width; i++ ) {
			b = font->p[index++];
			color = ((((fc & 0xFF) * b + (bc & 0xFF) * (256 - b)) >> 8) & 0xFF) |//Blue component
				((((fc & 0xFF00) * b + (bc & 0xFF00) * (256 - b)) >> 8)  & 0xFF00)|//Green component
				((((fc & 0xFF0000) * b + (bc & 0xFF0000) * (256 - b)) >> 8) & 0xFF0000); //Red component
			tglImagePutPixel(img, xo, yo, color);
			xo++;
		}
		index += font->char_width - actual_char_width;
		yo++;
	}
}

void tglImagePutString(TGLBITMAP *img, uint16_t x, uint16_t y, char* str, uint32_t fc, uint32_t bc, TGL_FONT *font, bool transparency) {

	short xp,yp;
    uint8_t cw;
    char chr;

    xp=x;
    yp=y;

    while ( *str != 0 ) {
        chr = *str++;
        if (chr < font->start_char || chr > font->end_char)
            continue;
        if ( chr == '\n' ) {
            xp = _gui.x_dim;
            continue;
        }
        cw = font->widths ? font->widths[chr - font->start_char] : font->char_width;

        if ( xp + cw > _gui.x_dim - 1 ) {
            xp = x;
            yp += font->char_height+_gui.char_v_space;
        }

        tglImagePutChar(img, xp, yp, chr, fc, bc, font, transparency);

        xp += cw + _gui.char_h_space;
    }
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
