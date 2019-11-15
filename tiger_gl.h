
#ifndef __TIGER_GL_H_
#define __TIGER_GL_H_

#include <FreeImage.h>
#include "ugui.h"

#define tglWidgetRegister(...) tglWidgetRegisterV(countArgs(#__VA_ARGS__), __VA_ARGS__)

#define TGL_AUTOUPDATE		1
#define TGL_NO_AUTOUPDATE	0

#define TOUCH_START		0x01
#define TOUCH_STOP		0x02
#define TOUCH_X			0x04
#define TOUCH_Y			0x08
#define TOUCH_PRESSURE	0x10
#define TOUCH_XMOVE		0x20
#define TOUCH_YMOVE		0x40

#define MAX_AREAS		64

#define TGL_COLOR_WHITE				0xffffff
#define TGL_COLOR_SILVER			0xC0C0C0
#define TGL_COLOR_GRAY				0x808080
#define TGL_COLOR_LIGHTGRAY			0xa0a0a0
#define TGL_COLOR_LIGHTERGRAY		0xc0c0c0
#define TGL_COLOR_DARKGRAY			0x505050
#define TGL_COLOR_BLACK				0x000000
#define TGL_COLOR_RED				0xff0000
#define TGL_COLOR_LIGHTRED			0xff8080
#define TGL_COLOR_LIGHTCORAL		0xf08080
#define TGL_COLOR_SALMON			0xFa8072
#define TGL_COLOR_MAROON			0x800000
#define TGL_COLOR_YELLOW			0xffff00
#define TGL_COLOR_OLIVE				0x808000
#define TGL_COLOR_LIME				0x00ff00
#define TGL_COLOR_GREEN				0x008000
#define TGL_COLOR_LIGHTGREEN		0x90ee90
#define TGL_COLOR_AQUA				0x00ffff
#define TGL_COLOR_CYAN				TGL_COLOR_AQUA
#define TGL_COLOR_TEAL				0x008080
#define TGL_COLOR_BLUE				0x0000ff
#define TGL_COLOR_SKYBLUE			0x87deeb
#define TGL_COLOR_LIGHTBLUE			0xadd8e6
#define TGL_COLOR_MEDIUMBLUE		0x0000cd
#define TGL_COLOR_STEELBLUE			0x4682b4
#define TGL_COLOR_NAVY				0x000080
#define TGL_COLOR_FUCHSIA			0xff00ff
#define TGL_COLOR_PURPLE			0x800080

#define TGLBITMAP	FIBITMAP
#define TGLRGB		RGBQUAD
#define TGL_FONT	UG_FONT

#define MAX_LLNAME      32
#define MAX_LLISTS      16


typedef enum {
	WIDGET_LINE,
	WIDGET_TEXTBOX,
	WIDGET_BUTTON,
	WIDGET_CHECKBOX,
	WIDGET_RADIO,
	WIDGET_LISTBOX,
	WIDGET_IMAGE,
	WIDGET_LABEL,
	WIDGET_PROGRESSBAR,
	WIDGET_SPINNER,
	WIDGET_MAIN_SCREEN
} WidgetType;

typedef enum {
	TOUCH_UP = 1,
	TOUCH_DOWN = 2
} TouchAction;

typedef struct _clipregion_ {
	bool isActive;
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
} ClipRegion;

typedef struct _tglWidget_ {
	WidgetType widgetType;
	bool checked;
	bool hasFocus;
	bool pbText;		// progress bar flag
	bool isRegistered;
	bool isPressed;
	uint16_t groupId;
	uint16_t widgetId;
	uint16_t pbNum;		// progress bar number
	uint16_t pbMax;		// progress bar max value
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	char *strList;		// used for Spinner and Listbox
	uint16_t cnt;		// used for Spinner and Listbox
	uint16_t len;		// used for Spinner and Listbox
	uint16_t selected;	// used for Spinner and Listbox
	uint16_t position;
	TouchAction touchAction;
	char *data;
	char *text;
	TGL_FONT *textFont;
	uint32_t fgColor;
	uint32_t bgColor;
	uint32_t pbColor;		// progressbar color
	uint32_t txtColor;		// text foreground color
	char *iconName;
	TGLBITMAP *icon;
	void (*eCallback)(struct _tglWidget_ *tw, uint16_t x, uint16_t y, uint16_t p);
	void (*paintWidget)(struct _tglWidget_ *tw, bool flag);
	void (*touchWidget)(struct _tglWidget_ *tw);
} TglWidget;

typedef struct _twdata_ {
	TglWidget *tw;
	struct _twdata_ *next;
} TwData;

typedef struct _twlist {
    int inUse;
    int itemCount;
    pthread_mutex_t llock;
    pthread_cond_t lcond;
    TwData *head;
    TwData *tail;
} TwList;

typedef struct _ldata {
	void *data;
	int dataSize;
    struct _ldata *next;
} Ldata;

typedef struct _LList {
    int inUse;
    int itemCount;
    void *data;
    pthread_mutex_t llock;
    pthread_cond_t lcond;
    Ldata *head;
    Ldata *tail;
} LList;

typedef struct _widgetList_ {
	struct _widgetList *prev;
	struct _widgetList *next;
	TglWidget *tw;
} TglWidgetList;

typedef struct _tglScreen_ {
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	uint16_t bpp;
	FIBITMAP *screen;
} TglScreen;

typedef enum {
	TGL_FILTER_BOX,
	TGL_FILTER_BILINEAR,
	TGL_FILTER_BSPLINE,
	TGL_FILTER_BICUBIC,
	TGL_FILTER_CATMULLROM,
	TGL_FILTER_LANCZOS3
} TglFilter;

typedef enum {
	TGL_LINE,
	TGL_TEXT,
	TGL_CIRCLE,
	TGL_ARC,
	TGL_IMAGE,
	TGL_RECT
} GuiType;

typedef struct _diryArea_ {
	GuiType type;
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
} DirtyArea;

typedef struct _saveArea_ {
	uint16_t id;
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
	uint8_t *area;
} SavedArea;

typedef struct _tglInfo_ {
    char *deviceName;
    int fbfd;               // FrameBuffer file descriptor
    int tty1;               // File discriptor for /dev/tty1 so I can hide cursor.

	char *touchDevice;

	int autoUpdate;
    uint16_t bpp;                // bits per pixel;
	uint16_t byteCnt;
    uint16_t width;
    uint16_t height;
    unsigned char *fbp;              // framebuffer memory pointer
    int fbSize;             // in bytes
    unsigned char *origScreen;		// original screen image
	TglScreen *rootScreen;
} TglInfo;

int countArgs(char *s);

int tglInit(char *device, uint16_t width, uint16_t height);
int tglFbOpen(char *device);
void tglSetAutoUpdate(int flag);
void tglFbClose(void);
unsigned char *tglFbGetFbp(void);
int tglFbGetWidth(void);
int tglFbGetHeight(void);
int tglFbGetBpp(void);
void tglFbUpdate(void);
// int tglFbTouchInit(char *device, uint16_t screenWidth, uint16_t screenHeight, uint16_t touchWidth, uint16_t touchHeight, bool pressure, bool rotate);
void tglTouchStopEvent(void);
void tglTouchGetEvent(void);
int tglFbAddScreen(void);
uint16_t *tglFbGetScreen(int id);
int tglFbDelScreen(int id);
void tglFbUpdateArea(unsigned char *buf, uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void tglFbSaveArea(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
void tglFbRestoreArea(void);
void tglFbPrintInfo(void);
void tglFindTouchDevice(char *deviceName);
int tglTouchInit(char *device, uint16_t screenWidth, uint16_t screenHeight, uint16_t touchWidth, uint16_t touchHeight, bool pressureFlag, bool rotate);
 
#define MAX_CQNAME	32
#define MAX_CQUEUES	16

#define CQ_NONBLOCK		0
#define CQ_BLOCK		1

typedef union _items_ {
	unsigned char c;
	unsigned short s;
	unsigned int i;
	unsigned long l;
	char *p;
} ItemType;

typedef struct _CQueue {
    volatile int cqItemCount;
	char cqName[MAX_CQNAME + 1];
	void **buffer;
	int blkSize;
	int arrSize;
	int growth;
	pthread_mutex_t cqLock;
	pthread_cond_t cqCond;
	int queIn;
	int queOut;
	ItemType array[0];		// Must be kept at end of structure.
} CQueue;

int cqInit(int cqueueCnt);
int cqCreate(const char *cqName, int arrSize);
int cqCreateDynamic(const char *cqName, int arrSize, int growth);
int cqGrow(int cqNum, int growth);
int cqSetGrowth(int cqNum, int growth);
int cqSetBuffer(int cqNum, void *buffer, int blkSize);
int cqAdd(int cqNum, ItemType *value);
int cqRemoveTimeout(int cqNum, ItemType *value, int block, int ms);
int cqRemove(int cqNum, ItemType *value, int block);
int cqRemoveTimed(int cqNum, ItemType *value, int timeout);
int cqDestroy(int queNum);
int cqCount(int queNum);
int cqArrSize(int queNum);
char *cqGetName(int queNum);
int cqGetNum(char *cqName);

void _addArea(WidgetType type, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void tglDrawSetClipRegion(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
void tglDrawUnsetClipRegion();
void tglDrawFillScreen(uint32_t c);
void tglDrawVideoImage(TglWidget *tw, TGLBITMAP *img);
void tglDrawImage(uint16_t x, uint16_t y, TGLBITMAP *img, bool transparency);
void tglDrawTransparentImage(uint16_t x, uint16_t y, TGLBITMAP *img);
void tglDrawLine(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint32_t c);
void tglDrawRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t c);
void tglDrawRoundRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t radius, uint32_t c);
void tglDrawFillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t c);
void tglDrawFillRoundRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t radius, uint32_t c);
void tglDrawCircle(uint16_t x, uint16_t y, uint16_t radius, uint32_t c);
void tglDrawFillCircle(uint16_t x, uint16_t y, uint16_t radius, uint32_t c);
void tglScreenPutChar(char c, uint16_t x, uint16_t y, uint32_t fc, uint32_t bc, bool transparency);
void tglScreenPutString(uint16_t x, uint16_t y, char *text, uint32_t fc, uint32_t bc, bool transparency);
void tglDrawMeshRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t c);
void tglDrawArc( uint16_t x, uint16_t y, uint16_t radius, int sec, uint32_t c);
void tglDrawPutChar(char c, uint16_t x, uint16_t y, uint32_t fc, uint32_t bc, bool transparency);
void tglDrawPutString(uint16_t x, uint16_t y, char *text, uint32_t fc, uint32_t bc, bool transparency);
void tglSetFgColor(uint32_t fg);
void tglSetBgColor(uint32_t bg);

TGLBITMAP *tglImageLoadMem(unsigned char *mem, int memLength, char *type);
TGLBITMAP *tglImageCreate(uint16_t width, uint16_t height, uint16_t bpp);
TGLBITMAP *tglImageLoad(char *fileName);
TGLBITMAP *tglImageComposite(TGLBITMAP *img, uint32_t fc);
int tglImageSave(TGLBITMAP *img, char *fileName);
int tglImageAppend(FILE *fd, char *img, int imgSize);
int tglImageSaveRaw(unsigned char *bits, uint16_t width, uint16_t height, uint16_t bpp, char *fileName);
int tglImageDelete(TGLBITMAP *img);
TGLBITMAP *tglImageClone(TGLBITMAP *img);
BYTE *tglImageGetScanLine(TGLBITMAP *img, uint16_t row);
unsigned tglImageGetBPP(TGLBITMAP *img);
unsigned char *tglImageGetBits(TGLBITMAP *img);
int tglImageGetType(TGLBITMAP *img);
int tglImageGetColorsUsed(TGLBITMAP *img);
int tglScreenImageGetBpp(TGLBITMAP *img);
int tglImageGetWidth(TGLBITMAP *img);
int tglImageGetHeight(TGLBITMAP *img);
unsigned tglImageGetPitch(TGLBITMAP *img);
unsigned tglImageGetLine(TGLBITMAP *img);
TGLBITMAP *tglImageRotate(TGLBITMAP *img, double angle);
int tglImageFlipHorizontal(TGLBITMAP *img);
int tglImageFlipVertical(TGLBITMAP *img);
unsigned tglImageGetSize(TGLBITMAP *img);
TGLBITMAP *tglImageRescale(TGLBITMAP *img, uint16_t width, uint16_t height, TglFilter filter);
void tglImagePrintInfo(TGLBITMAP *img, char *title);
TGLBITMAP *tglImageConvertTo32(TGLBITMAP *img);
void tglImageSetTransparent(TGLBITMAP *img, bool flag);
void tglImageSetTransTable(TGLBITMAP *img, BYTE *table, int count);
void tglImageGetBg(TGLBITMAP *img, TGLRGB *rgb);
void tglImageFill(TGLBITMAP *img, uint32_t c);
void tglImageDrawLine(TGLBITMAP *img, uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t c);
void tglImagePutChar(TGLBITMAP *img, uint16_t x, uint16_t y, char text, uint32_t fc, uint32_t bc, TGL_FONT *font, bool transparency);
void tglImagePutString(TGLBITMAP *img, uint16_t x, uint16_t y, char *text, uint32_t fc, uint32_t bc, TGL_FONT *font, bool transparency);

int tglScreenCreate(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t bpp);
int tglScreenLoad(char *fileName);
int tglScreenSave(char *fileName);
int tglScreenDelete();
TGLBITMAP *tglScreenGetBitmap();
BYTE *tglScreenGetPixels();
BYTE *tglScreenGetScanLine(uint16_t row);
int tglScreenGetType();
int tglScreenGetColorsUsed();
int tglScreenGetBpp();
int tglScreenGetWidth();
int tglScreenGetHeight();
unsigned tglScreenGetPitch();
unsigned tglScreenGetLine();
int tglScreenRotate(double angle);
int tglScreenFlipHorizontal();
int tglScreenFlipVertical();
unsigned tglScreenGetSize();
int tglScreenRescale(uint16_t width, uint16_t height, TglFilter filter);
void tglScreenPrintInfo(char *title);
void tglScreenSetFgColor(uint32_t tglColor);
void tglScreenSetBgColor(uint32_t tglColor);
void tglScreenLine(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint32_t c);
void tglScreenRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t c);
void tglScreenRoundRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t radius, uint32_t c);
void tglScreenFillRect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t c);
void tglScreenFillRoundRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t radius, uint32_t c);
void tglScreenCircle(uint16_t x, uint16_t y, uint16_t radius, uint32_t c);
void tglScreenFillCircle(uint16_t x, uint16_t y, uint16_t radius, uint32_t c);
void tglScreenPutPixel(short x, short y, uint32_t c, bool transparency);
void tglScreenPutChar(char c, uint16_t x, uint16_t y, uint32_t fc, uint32_t bc, bool transparency);
void tglScreenPutString(uint16_t x, uint16_t y, char *text, uint32_t fc, uint32_t bc, bool transparency);
void tglScreenFill(uint32_t color);
void tglScreenMeshRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t c);
void tglScreenArc( uint16_t x, uint16_t y, uint16_t radius, int sec, uint32_t c);
void tglScreenSetFont(char *fontName);
void tglScreenUpdate();

void tglWidgetDelete(TglWidget *tw);
TglWidget *tglWidgetImage(uint16_t xs, uint16_t ys, uint16_t width, uint16_t height);
TglWidget *tglWidgetButton(char *text, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
TglWidget *tglWidgetLabel(char *text, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
TglWidget *tglWidgetCheckbox(char *text, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
TglWidget *tglWidgetTextbox(char *text, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
TglWidget *tglWidgetRadio(char *text, uint16_t x, uint16_t y, uint16_t width, uint16_t height);
TglWidget *tglWidgetProgressBar(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bool pbText, uint32_t c);
TglWidget *tglWidgetSpinner(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *strList);
// TglWidget *tglWidgetListbox(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *strList);
// void tglWidgetSetListboxPosition(TglWidget *tw, uint16_t position);
void tglWidgetSetSelection(TglWidget *tw, uint16_t num);
void tglWidgetSetSpinnerList(TglWidget *tw, char *spList);
void tglWidgetSetProgressBarColor(TglWidget *tw, uint32_t c);
void tglWidgetSetProgressBarNum(TglWidget *tw, uint16_t num);
void tglWidgetSetButtonText(TglWidget *tw, char *text);
void tglWidgetSetSelected(TglWidget *tw, bool selected);
void tglWidgetSetRadioGroup(TglWidget *tw, uint16_t groupId);
void tglWidgetSetFont(TglWidget *tw, char *fontName);
void tglWidgetSetLabelText(TglWidget *tw, char *text);
void tglWidgetSetCheckboxText(TglWidget *tw, char *text);
void tglWidgetSetRadioText(TglWidget *tw, char *text);
void tglWidgetSetFgColor(TglWidget *tw, uint32_t fgColor);
void tglWidgetSetBgColor(TglWidget *tw, uint32_t bgColor);
void tglWidgetSetFgBgColor(TglWidget *tw, uint32_t fgColor, uint32_t bgColor);
char *tglWidgetGetSelection(TglWidget *tw);
int tglWidgetRegisterV(int count, ...);
int tglIsInside(TglWidget *tw, uint16_t x, uint16_t y);
void tglWidgetAddCallback(TglWidget *tw, void (*eCallback)(struct _tglWidget_ *tw, uint16_t x, uint16_t y, uint16_t p), TouchAction action);
void tglWidgetEvent(uint16_t x, uint16_t y, uint16_t p, uint16_t t, uint16_t c);
void tglWidgetAddIcon(TglWidget *tw, char *iconName);
void tglWidgetSetData(TglWidget *tw, char * data);
char *tglWidgetGetData(TglWidget *tw);

LList *llCreate();
int llAdd(LList *lp, const void *data, int dataSize);
void *llRemove(LList *lp, bool block, int *dataSize);
int llCount(LList *lp);
int llDestroy(LList *lp);

TwList *twCreate();
int twAdd(TwList *lp, TglWidget *tw);
int twFind(TwList *lp, int id);
TglWidget *twRemove(TwList *lp, bool block);
int twCount(TwList *lp);
TglWidget *twList(TwList *lp, int *length);
int twDestroy(TwList *lp);

#include "timer.h"

#endif
