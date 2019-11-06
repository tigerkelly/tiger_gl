
# tiger_gl - General graphical library for RPI

The tiger_gl library does not use X11, it writes directly to the Linux framebuffer.

![Image of Tiger_gl](images/tiger_gl.png)

### Building library

On a Raspberry Pi, should run on any version, you will need to install the follwoing libraries and programs.

	- sudo apt-get install input-utils
	- sudo apt-get install libfreeimage3 libfreeimage-dev
	- sudo apt-get install libpng-dev
	- sudo apt-get install evtest

Once installed you should be able to type **make** to build the library.
