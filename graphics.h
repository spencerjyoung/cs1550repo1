/*
	Spencer Young
	Peoplesoft: 4063872
	id: sjy13
	CS1550
	Disclaimer: some of the code was reused from my last semester when I took CS1550 with Dr. Farnan
*/
/*
	A graphics.h header file which defines prototypes for all the “public” functions, your color_t typedef, and your RGB conversion macro.
*/
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <unistd.h> 
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/kernel.h>
#include <sys/select.h>
#include <termios.h>
#include <sys/stat.h>


typedef unsigned short color_t;

// RGB conversion macro
#define RGB(r, g, b) ((color_t) (r << 11) | (g << 5) | (b))

//Declaration of function prototypes 
void init_graphics();//done
void exit_graphics();//done
void clear_screen(void *img);//done
char getkey();//done
void sleep_ms(long ms);//done
void *new_offscreen_buffer(); //done
void blit(void *src); //done
void draw_pixel(void *img, int x, int y, color_t color);//done
void draw_line(void *img, int x1, int y1, int width, int height, color_t c); //done
#endif