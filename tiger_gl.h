
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
	WIDGET_LABEL
} WidgetType;

typedef enum {
	TOUCH_UP = 1,
	TOUCH_DOWN = 2
} TouchAction;

typedef struct _tglWidget_ {
	WidgetType widgetType;
	bool inUse;
	bool checked;
	bool hasFocus;
	short groupId;
	short widgetId;
	int x;
	int y;
	int width;
	int height;
	TouchAction touchAction;
	char *data;
	char *text;
	UG_FONT *textFont;
	int fgColor;
	int bgColor;
	char *iconName;
	TGLBITMAP *icon;
	void (*eCallback)(struct _tglWidget_ *tw, int x, int y, int p);
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
    //TglWidget *list;		// filled by the twList() function.
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
	int x;
	int y;
	int width;
	int height;
	int bpp;
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
	int x;
	int y;
	int width;
	int height;
} DirtyArea;

typedef struct _saveArea_ {
	int id;
	int x;
	int y;
	int width;
	int height;
	unsigned char *area;
} SavedArea;

typedef struct _tglInfo_ {
    char *deviceName;
    int fbfd;               // FrameBuffer file descriptor
    int tty1;               // File discriptor for /dev/tty1 so I can hide cursor.

	char *touchDevice;

	int autoUpdate;
    int bpp;                // bits per pixel;
	int byteCnt;
    int width;
    int height;
    unsigned char *fbp;              // framebuffer memory pointer
    int fbSize;             // in bytes
    unsigned char *origScreen;		// original screen image
	TglScreen *rootScreen;
} TglInfo;

int countArgs(char *s);

int tglInit(char *device, int width, int height);
int tglFbOpen(char *device);
void tglSetAutoUpdate(int flag);
void tglFbClose(void);
unsigned char *tglFbGetFbp(void);
int tglFbGetWidth(void);
int tglFbGetHeight(void);
int tglFbGetBpp(void);
void tglFbUpdate(void);
// int tglFbTouchInit(char *device, int screenWidth, int screenHeight, int touchWidth, int touchHeight, int pressure, int rotate);
void tglTouchStopEvent(void);
void tglTouchGetEvent(void);
int tglFbAddScreen(void);
unsigned int *tglFbGetScreen(int id);
int tglFbDelScreen(int id);
void tglFbUpdateArea(unsigned char *buf, int x, int y, int w, int h);
void tglFbSaveArea(int x, int y, int w, int h);
void tglFbRestoreArea(void);
void tglFbPrintInfo(void);
void tglFindTouchDevice(char *deviceName);
int tglTouchInit(char *device, int screenWidth, int screenHeight, int touchWidth, int touchHeight, int pressureFlag, int rotate);
 
#define MAX_CQNAME	32
#define MAX_CQUEUES	16

#define CQ_NONBLOCK	0
#define CQ_BLOCK	1

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
int cqRemove(int cqNum, ItemType *value, int block);
int cqRemoveTimed(int cqNum, ItemType *value, int timeout);
int cqDestroy(int queNum);
int cqCount(int queNum);
int cqArrSize(int queNum);
char *cqGetName(int queNum);
int cqGetNum(char *cqName);

void _addArea(WidgetType type, int x, int y, int width, int height);
void tglDrawVideoImage(TglWidget *tw, TGLBITMAP *img);
void tglDrawImage(int x, int y, TGLBITMAP *img, bool transparency);
void tglDrawTransparentImage(int x, int y, TGLBITMAP *img);
void tglDrawLine(int xs, int ys, int xe, int ye, unsigned int c);
void tglDrawRect(int x, int y, int width, int height, unsigned int c);
void tglDrawRoundRect(int x, int y, int width, int height, int radius, unsigned int c);
void tglDrawFillRect(int x, int y, int width, int height, unsigned int c);
void tglDrawFillRoundRect(int x, int y, int width, int height, int radius, unsigned int c);
void tglDrawCircle(int x, int y, int radius, unsigned int c);
void tglDrawFillCircle(int x, int y, int radius, unsigned int c);
void tglScreenPutChar(char c, int x, int y, unsigned int fc, unsigned int bc);
void tglScreenPutString(int x, int y, char *text, unsigned int fc, unsigned int bc);
void tglDrawMeshRect(int x, int y, int width, int height, unsigned int c);
void tglDrawArc( int x, int y, int radius, int sec, unsigned int c);
void tglDrawPutChar(char c, int x, int y, unsigned int fc, unsigned int bc);
void tglDrawPutString(int x, int y, char *text, unsigned int fc, unsigned int bc);
void tglSetFgColor(int fg);
void tglSetBgColor(int bg);

TGLBITMAP *tglImageLoadMem(unsigned char *mem, int memLength);
TGLBITMAP *tglImageCreate(int x, int y, int width, int height, int bpp);
TGLBITMAP *tglImageLoad(char *fileName);
TGLBITMAP *tglImageComposite(TGLBITMAP *img, unsigned int fc);
int tglImageSave(TGLBITMAP *img, char *fileName);
int tglImageAppend(FILE *fd, char *img, int imgSize);
int tglImageSaveRaw(unsigned char *bits, int width, int height, int bpp, char *fileName);
int tglImageDelete(TGLBITMAP *img);
TGLBITMAP *tglImageClone(TGLBITMAP *img);
BYTE *tglImageGetScanLine(TGLBITMAP *img, int row);
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
TGLBITMAP *tglImageRescale(TGLBITMAP *img, int width, int height, TglFilter filter);
void tglImagePrintInfo(TGLBITMAP *img, char *title);
TGLBITMAP *tglImageConvertTo32(TGLBITMAP *img);
void tglImageSetTransparent(TGLBITMAP *img, bool flag);
void tglImageSetTransTable(TGLBITMAP *img, BYTE *table, int count);
void tglImageGetBg(TGLBITMAP *img, TGLRGB *rgb);

int tglScreenCreate(int x, int y, int width, int height, int bpp);
int tglScreenLoad(char *fileName);
int tglScreenSave(char *fileName);
int tglScreenDelete();
TGLBITMAP *tglScreenGetBitmap();
BYTE *tglScreenGetPixels();
BYTE *tglScreenGetScanLine(int row);
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
int tglScreenRescale(int width, int height, TglFilter filter);
void tglScreenPrintInfo(char *title);
void tglScreenSetFgColor(unsigned int tglColor);
void tglScreenSetBgColor(unsigned int tglColor);
void tglScreenLine(int xs, int ys, int xe, int ye, unsigned int c);
void tglScreenRect(int x, int y, int width, int height, unsigned int c);
void tglScreenRoundRect(int x, int y, int width, int height, int radius, unsigned int c);
void tglScreenFillRect(int x, int y, int width, int height, unsigned int c);
void tglScreenFillRoundRect(int x, int y, int width, int height, int radius, unsigned int c);
void tglScreenCircle(int x, int y, int radius, unsigned int c);
void tglScreenFillCircle(int x, int y, int radius, unsigned int c);
void tglScreenPutPixel(short x, short y, unsigned int c);
void tglScreenPutChar(char c, int x, int y, unsigned int fc, unsigned int bc);
void tglScreenPutString(int x, int y, char *text, unsigned int fc, unsigned int bc);
void tglScreenFill(unsigned int color);
void tglScreenMeshRect(int x, int y, int width, int height, unsigned int c);
void tglScreenArc( int x, int y, int radius, int sec, unsigned int c);
void tglScreenSetFont(char *fontName);
void tglScreenUpdate();

void tglWidgetDelete(TglWidget *tw);
TglWidget *tglWidgetImage(int xs, int ys, int width, int height);
TglWidget *tglWidgetButton(char *text, int x, int y, int width, int height);
TglWidget *tglWidgetLabel(char *text, int x, int y, int width, int height);
TglWidget *tglWidgetCheckbox(char *text, int x, int y, int width, int height);
// TglWidget *tglWidgetTextbox(char *text, int x, int y, int width, int height);
TglWidget *tglWidgetRadio(char *text, int x, int y, int width, int height);
void tglWidgetSetButtonText(TglWidget *tw, char *text);
void tglWidgetSetSelected(TglWidget *tw, bool selected);
void tglWidgetSetRadioGroup(TglWidget *tw, short groupId);
void tglWidgetSetFont(TglWidget *tw, char *fontName);
void tglWidgetSetLabelText(TglWidget *tw, char *text);
void tglWidgetSetCheckboxText(TglWidget *tw, char *text);
void tglWidgetSetRadioText(TglWidget *tw, char *text);
void tglWidgetSetFgColor(TglWidget *tw, int fgColor);
void tglWidgetSetBgColor(TglWidget *tw, int bgColor);
void tglWidgetSetFgBgColor(TglWidget *tw, int fgColor, int bgColor);
TglWidget *tglWidgetTextbox(char *text, int x, int y, int width, int height);
int tglWidgetRegisterV(int count, ...);
int tglIsInside(TglWidget *tw, int x, int y);
void tglWidgetAddCallback(TglWidget *tw, void (*eCallback)(struct _tglWidget_ *tw, int x, int y, int p), TouchAction action);
void tglWidgetEvent(int x, int y, int p, int t);
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
