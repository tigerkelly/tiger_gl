
# tiger_gl - General graphical library for RPI

The tiger_gl library does not use X11, it writes directly to the Linux framebuffer.

This library was inspired by the **[Littlevgl](https://github.com/littlevgl)** and **[UGUI](https://github.com/achimdoebler/UGUI)** libraries which are very good libraries.

Currently this library has the features I need for my projects but I will add new features as needed.  If yow have a need for a new feature or added a new feature to **tiger_gl** send an email to **rkwiles@twc.com** and let me know.

![Image of Tiger_gl](images/tiger_gl.png)

### Tiger_gl Features:

	- No X11 required.
	- Simple to use.
	- All code is in C.
	- Develop for Raspberry PI
	- Image Widget
	- Label Widget
	- Checkbox widget
	- Radio buttons with grouping
	- Touch screen support
	- Many simple font sizes
	- Support for video
	- Auto updating screen. (Only draws what has changed)

The **tiger_gl** library uses two screen buffers. The first screen area is what the **tiger_gl** library updates.  The second screen is the linux frame buffer which is automatically updated from the first screen.  I only update what has changed from the first screen to the linux frame buffer, cutting down on the memory copies.


### Files and API

The library is divdied up into groups:

	- General screen functions. (screen_gl.c)
	- Primartive drawing functions. (draw_gl.c)
	- Main library functions. (tiger_gl.c)
	- Touch screen functions. (touch_gl.c)
	- Widget fucntions. (widget_gl.c)
	- Ugui functions. (ugui.c) I am using the primative drawing and font functions of ugui.

#### Tiger_gl API

Function | Description
-------- | -----------
int tglInit(char *device, int width, int height) | First call to initialize the tiger_gl library.
int tglFbOpen(char *device) | Opens the linux frame buffer device.  Normally called by the tglIinit function.
void tglSetAutoUpdate(int flag) | Enables or disable auto updating.
void tglFbPrintInfo(void) | Prints information about FB.
void tglFbClose(void) | Restores screen and closes the frame buffer device.
unsigned char *tglFbGetFbp(void) | Returns the linux frame buffer pointer to memory.
int tglFbGetWidth(void) | Returns the screens width in pixels.
int tglFbGetHeight(void) | Returns the screens height in pixels.
int tglFbGetBpp(void) | Returns the screens bits per pixel. (always 32 bits)
void tglFbUpdate(void) | Updates the complete screen, normally only called by the tiger_gl library.
void tglFbUpdateArea(unsigned char *buf, int bx, int by, int bw, int bh) | Updates a rectangle area on the screen. Normally called by the update thread.
void tglFindTouchDevice(char *deviceName) | Use the lsinput program to find the touch screen device.  Currently supports 3 screen types, see code for screens.

Function | Description
-------- | -----------
int tglScreenCreate(int x, int y, int width, int height, int bpp) | Used to create the first screen.
int tglScreenLoad(char *fileName) | Used to load a screen image directly to the screen.
int tglScreenSave(char *fileName) | Save the current screen to an image file.
int tglScreeDelete(void) | Deletes the screen. Not normally called.
TGLBITMAP *tglScreengetBitmap(void) | Returns the screen image as a pointer to TGLBITMAP.
BYTE *tglScreeGetPixels(void) | Returns the pointer to the screem image pixels.
BYTE *tglScreenGetScanLine(int row) | Returns a pointer to the row given.
int tglScreenGetType(void) | Returns the image type of the screen.
int tglScreenGetColorsUsed(void)| Returns the number of colors used.
int tglScreenGetBpp(void) | Returns pointer to screen image.
int tglScreenGetWidth(void) | Returns screen width in pixels.
int tglScreenGetHeight(void) | Returns screen height in pixels.
int tglScreenGetPitch(void) | Returns width of screen in bytes rounded to the next 32 bit boundary.
int tglScreenGetLine(void) | Get scan line length in pixels
int tglScreenRotate(double angle | Rotate screen by angle.
int tglScreenFlipHorizontal(void) | Flips screen horizontal
int tglScreenFlipVertical(void) | Flips screen vertical
int tglScreenGetSize(void) | Returns the size of the DIB-element
int tglScreenRescale(char *fileName) | Rescales the image.
void tglScreenPrintInfo(char *title) | Print screen information.
void tglScreenPutPixel(int x, int y, int c) | Called by all functons needing to draw something to the screen.
void tglScreePutChar(char c, int x, int y, unsigned int fc, unsigned int bc | Draws a single characrter to the screen.
void tglScreePutString(int x, int y, char *text, unsigned int fc, unsigned int bc) | Draws a string of characrters to the screen.
void tglScreenFill(unsigned int c) | fills screen with a color.


Function | Description
-------- | -----------
void tglDrawLine(int xs, int ys, int xe, int ye, unsigned int c) | Draws a line on the screen.
void tglDrawRect(int x, int y, int width, int height, unsigned int c) | Draws a rectangle on the screen.
void tglDrawRoudRect(int x, int y, int width, int height, int radius, unsigned int c) | Draws a rounded rectangle on the screen.
void tglDrawFillRect(int x, int y, int width, int height, unsigned int c) | Draws a filled rectanlge on the screen.
void tglDrawFillRoudRect(int x, int y, int width, int height, unsigned int c) | Draws a filled rounded rectangle on the screen.
void tglDrawCircle(int x, int y, int radius, unsigned int c) | Draws a circle on the screen.
void tglDrawFillCircle(int x, int y, int radius, unsigned int c) | Draws a filled circle on the screen.
void tglDrawMeshRect(int x, int y, int width, int height, unsigned int c) | Draws a meshed rectangle on the screen.
void tglDrawArc( int x, int y, int radius, int sec, unsigned int c) | Draws a arc on the screen.
void tglDrawImage(int x, int y, TGLBITMAP *img, bool transparency) | Draws an image to the screen with ot without transparency.
void tglDrawVideoImage(TglWidget *tw, TGLBITMAP *img) | Used to support drawing frames of video to the screen.

Function | Description
-------- | -----------
TGLBITMAP *tglImageCreate(int x, int y, int width, int height, int bpp) | Allocate an image in memory.
TGLBITMAP *tglImageLoad(char *fileName) | Load an image into memory.
void tglImageSetTransparent(TGLBITMAP *img, bool flag) | Set an image to support tranparency.
void tglImageSetTransTable(TGLBITMAP *img, BYTE *table, int count) | Set images transparency table.
void tglImageGetBg(TGLBITMAP *img, TGLRGB *rgb) | Get images background color.
TGLBITMAP *tglImageLoadMem(unsigned char *mem, int memLength) | Load an image from memory.
int tglImageSave(TGLBITMAP * img, char *fileName) | Save an image to disk.
int tglImageDelete(TGLBITMAP *img) | Delete an image in memory.
int tglImageSaveRaw(unsigned char *bits, int width, int height, int bpp, char *fileName) | Save image to disk in raw format.
TGLBITMAP *tglImageComposite(TGLBITMAP *img, unsigned int fc) | Adds a composite to the image.
TGLBITMAP *tglImageClone(TGLBITMAP *img) | Clone an image.
BYTE *tglImageGetScanLine(TGLBITMAP *img, int row) | Get pointer to image row.
unisgned char *tglImageGetBits(TGLBITMAP *img) | Get pointer to image data.
unsigned tglImageGetBPP(TGLBITMAP *img) | Get images bit per pixel.
int tglImageGetType(TGLBITMAP *img) | Get image type.
int tglImageGetColorsUsed(TGLBITMAP *img)  | Get number of colors used by image.
int tglImageGetWidth(TGLBITMAP *img)| Get width of image.
int tglImageGetHeight(TGLBITMAP *img) | Get height of image.
nt unsigned tglImageGetPitch(TGLBITMAP *img) | Get pitch of image.
unsigned tglImageGetLine(TGLBITMAP *img) | Get scan line size in bytes.
TGLBITMAP *tglImageRotate(TGLBITMAP *img, double angle) | Rotate image by angle.
int tglImageFlipHorizontal(TGLBITMAP *img) | Flip image horizontal
int tglImageFlipVertical(TGLBITMAP *img) | Flip image vertical
unsigned tglImageGetSize(TGLBITMAP *img) | Get size of image.
TGLBITMAP *tglImageRescale(TGLBITMAP *img, int width, int height, TglFilter filter) | Rescale image.
TGLBITMAP *tglImageConvertTo32(TGLBITMAP *img) | Convert image to 32 bpp
void tglImagerPrintInfo(TGLBITMAP *img, char *title) | Print information about image. Normally called by tiger_gl library.

Function | Description
-------- | -----------
int tglTouchInit(char *device, int screenWidth, int screenHeight, int touchWidth, int touchHeight, int pressureFlag, int rotate) | Noramlly called by tglInit()
void tglTouchGetEvent(void) | Gets the next touch event.  Normally called by touch thread.

Function | Description
-------- | -----------
int tglWidgetRegister(...) | A #define which calls the tglWidgetRegisterV function. Comma seperated list of TglWidget *.
int tglWidgetRegisterV(int count, ...) | Registers widgets and calls their paint routines.  Use tglWidgetRegister function instead.
void tglWidgetSetData(TglWidget *tw, char * data) | Sets data for a widget, used by the programmer to pass data.
void char *tglWidgetGetData(TglWidget *tw) | Retrives the data of a widget.
tglWidgetEvent(int x, int y, int p, int t) | Used by the touch thread to notify a widget of an event.
void tglWidgetDelete(TglWidget *tw) | Deletes a widget.
TglWidget *tglWidgetImage(int x, int y, int width, int height) | Create an image widget.
void tglWidgetAddCallback(TglWidget *tw, void (*eCallback)(struct _tglWidget_ *tw, int x, int y, int p), TouchAction action) | Adds a callback to the widget. see example program tiger_gl-test
void tglWidgetAddIcon(TglWidget *tw, char *iconName) | Add an Icon to the widget if it supports it.
void tglWidgetSetFont(TglWidget *tw, char *fontName) | Set the font used by a widget.
void tglWidgetSetFgColor(TglWidget *tw, int fgColor) | Set foreground color of widget.
void tglWidgetSetBgColor(TglWidget *tw, int bgColor) | Set background color of widget.
void tglWidgetSetFgBgColor(TglWidget *tw, int fgColor, TglWidget *tw, int bgColor) | Set foreground and background colors of widget.
void tglWidgetSetButtonText(TglWidget *tw, char *text) | Set the text of the button widget.
TglWidget *tglWidgetButton(char *text, int x, int y, int width, int height) | Create a button widget.
void tglWidgetSetLabelText(TglWidget *tw, char *text)| Set text of a label widget.
TglWidget *tglWidgetLabel(char *text, int x, int y, int width, int height) | Create a label widget.
TglWidget *tglWidgetCheckbox(char *text, int x, int y, int width, int height) | Create a checkbox widget.
tglWidgetSetCheckboxText(TglWidget *tw, char *text) | Set the text of the checkbox widget.
TglWidget *tglWidgetRadio(char *text, int x, int y, int width, int height) | Create a radio button.
void tglWidgetSetRadioText(char *text, int x, int y, int width, int height) | Set text of a radio button.
void tglWidgetSetRadioGroup(TglWidget *tw, short groupId) | Set radio button group id.
void tglWidgetSetSelected(TglWidget *tw, bool selected) | Set widget as selected.


### Building library

On a Raspberry Pi, should run on any version, you will need to install the following libraries and programs.

	- sudo apt-get install input-utils
	- sudo apt-get install libfreeimage3 libfreeimage-dev
	- sudo apt-get install libpng-dev
	- sudo apt-get install evtest

You will also need to install the **[utils](https://github.com/tigerkelly/utils)** library.  (see that repository for install instructions.)

A test program has been written **[tiger_gl-test](https://github.com/tigerkelly/tiger_gl-test)**

Once installed you should be able to type **make** to build the library.

I have only tested this on a Raspberry Pi using the NOOBS default OS.
