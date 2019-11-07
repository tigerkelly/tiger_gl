
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
	tglInit | First call ti initialize the **tiger_gl** library.
	tglFbOpen | Opens the linux frame buffer device.  Normally called by the **tglIint** function.
	tglSetAutoUpdate | Enables or disable auto update.
	tglFbClose | Restores screen and closes the frame buffer device.
	tglFbGetFbp | Returns the linux frame buffer pointer to memory.
	tglFbGetWidth | Returns the screens width in pixels.
	tglFbGetHeight | Returns the screens height in pixels.
	tglFbGetBpp | Returns the screens bits per pixel. (always 32 bits)
	tglFbUpdate | Updates the complete screen, normally only called by the **tiger_gl** library.
	tglFbUpdateArea | Updates a rectangle area on the screen. Normally called by the update thread.
	tglFindTouchDevice | Use the lsinput program to find the touch screen device.  Currently supports 3 screen types, see code for screens.

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
