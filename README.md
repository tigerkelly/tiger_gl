
# tiger_gl - General graphical library for RPI

The tiger_gl library does not use X11, it writes directly to the Linux framebuffer.

![Image of Tiger_gl](images/tiger_gl.png)

### Building library

On a Raspberry Pi, should run on any version, you will need to install the following libraries and programs.

	- sudo apt-get install input-utils
	- sudo apt-get install libfreeimage3 libfreeimage-dev
	- sudo apt-get install libpng-dev
	- sudo apt-get install evtest

You will also need to install the **[utils](https://github.com/tigerkelly/utils)** library at my GitHub account.  (see that repository for install instructions.)

Once installed you should be able to type **make** to build the library.

I have only tested this on a Raspberry Pi using the NOOBS default OS.
