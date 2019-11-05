
CC=gcc

SRC=tiger_gl.c touch_gl.c screen_gl.c image_gl.c widgets_gl.c draw_gl.c ugui.c ugui_fonts.c cqueue.c llist.c twlist.c timer.c countargs.c

# FTCFLAGS=-I/usr/local/include/freetype2 -I/usr/include/libpng16 -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib/arm-linux-gnueabihf/glib-2.0/include

CFLAGS =-std=gnu99 -g -Wall -I./ -I./utils/incs -I/usr/include/directfb -I/usr/include/directfb/direct -I/usr/lib/gcc/arm-linux-gnueabihf/8/include

ARC=libtiger_gl.a

all: $(ARC)

$(ARC): tiger_gl.o touch_gl.o screen_gl.o image_gl.o widgets_gl.o draw_gl.o ugui.o ugui_fonts.o cqueue.o llist.o twlist.o timer.o countargs.o
	$(AR) -r $(ARC) tiger_gl.o touch_gl.o screen_gl.o image_gl.o widgets_gl.o \
		draw_gl.o ugui.o ugui_fonts.o cqueue.o llist.o twlist.o timer.o countargs.o

tiger_gl.o: tiger_gl.c tiger_gl.h
	$(CC) $(CFLAGS) -c $< -o $@

ugui.o: ugui.c ugui.h
	$(CC) $(CFLAGS) -c $< -o $@

toucg_gl.o: touch_gl.c toucg.h
	$(CC) $(CFLAGS) -c $< -o $@

cqueue.o: cqueue.c myatomic.h
	$(CC) $(CFLAGS) -c $< -o $@

timer.o: timer.c timer.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf tiger_gl.o touch_gl.o screen_gl.o image_gl.o widgets_gl.o draw_gl.o \
		ugui.o ugui_fonts.o cqueue.o llist.o twlist.o timer.o countargs.o $(ARC)

