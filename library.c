#include "graphics.h" 
/*
	Spencer Young
	Peoplesoft: 4063872
	id: sjy13
	CS1550
	Disclaimer: some of the code was reused from my last semester when I took CS1550 with Dr. Farnan
*/

int size_of_display; //Size of the display's space when mapping the framebuffer to our address space
int framebuffer_desc; //The framebuffer device file descriptor
void* framebuffer; //The actual address of the framebuffer in memory
/*
http://man7.org/linux/man-pages/man3/termios.3.html
The termios functions describe a general terminal interface that is
provided to control asynchronous communications ports.
*/
struct termios terminal_settings;
//https://www.kernel.org/doc/Documentation/fb/api.txt
/*
struct fb_var_screeninfo stores device independent changeable information
about a frame buffer device, its current format and video mode, as well as
other miscellaneous parameters.
*/
struct fb_var_screeninfo screen_var_info;
/*
struct fb_fix_screeninfo stores device independent unchangeable information
about the frame buffer device and the current format. Those information can't
be directly modified by applications, but can be changed by the driver when an
application modifies the format.
*/
struct fb_fix_screeninfo screen_fix_info;

//system calls used open, ioctl, mmap
void init_graphics(){
	//opening the framebuffer and getting file descpriptor accessing thorugh special device nodes
	//https://www.kernel.org/doc/Documentation/fb/framebuffer.txt
	framebuffer_desc = open("/dev/fb0", O_RDWR);
	
	//The ioctl() function manipulates the underlying device parameters of special files
	//http://www.tutorialspoint.com/unix_system_calls/ioctl.htm
	//To get the screen size and bits per pixels, we can use a special system call: ioctl. 
	//This system call is used to query and set parameters for almost any device connected to the system
	//give back a struct fb_fix_screeninfo from which we can determine the bit depth.
	ioctl(framebuffer_desc, FBIOGET_FSCREENINFO, &screen_fix_info);
	//give back a struct fb_var_screeninfo that will give the virtual resolution
	ioctl(framebuffer_desc, FBIOGET_VSCREENINFO, &screen_var_info);
	
	//setting the display size
	size_of_display = screen_var_info.yres_virtual * screen_fix_info.line_length;
	
	//Mapping the frame buffer in memory
	framebuffer = (color_t*) mmap(NULL, size_of_display, PROT_READ | PROT_WRITE, MAP_SHARED, framebuffer_desc, 0); 
	
	// Clearing the screen with an ascii escape character
    write(STDIN_FILENO, "\033[2J", 4);

	//getting the current terminal settings
	ioctl(1, TCGETS, &terminal_settings);
	//turnning Echo off
	terminal_settings.c_lflag &= ~ECHO;
	//turning Icanon off
	terminal_settings.c_lflag &= ~ICANON;
	//setting the new terminal settings
	ioctl(1, TCSETS, &terminal_settings); //Set the new terminal settings
}//end of the init_graphics method
//FINISHED

//system calls used ioctl
void exit_graphics(){
	ioctl(1, TCGETS, &terminal_settings); 
	//turnning Echo on
	terminal_settings.c_lflag |= ECHO;
	//turning Icanon on
	terminal_settings.c_lflag |= ICANON;
	//confirming the new settings
	ioctl(1, TCSETS, &terminal_settings); 
	//despite many thingsthings will be cleaned up automatically if we forget,
	//for instance files will be closed and memory can be unmapped but still manually using close() and munmap().
	munmap(framebuffer, size_of_display); 
	close(framebuffer_desc); 
}//end of the exit_graphics method
//FINISHED

//system calls used: none
void clear_screen(void *img){
	//We can print the string "\033[2J" to tell the terminal to clear itself.
	/*
	clear_screen()
	When we want to blank out our offscreen buffer or our framebuffer, we can just copy over every byte with the value zero.
	So, much like our blit(), loop over the image buffer parameter and set each byte to zero.
	*/
	unsigned long i;
	for(i = 0; i < size_of_display; i++){
		  ((char *)img)[i] = 0;
	}
}//end of the clear_screen method
//FINISHED


//system calls used: select and read
char getkey(){
	//http://man7.org/linux/man-pages/man2/select.2.html
	//below was heavily referenceing using the provided example in the man pages from the url above
	fd_set rfds;
	struct timeval tv;
	int retval;
	char buffered_char = '\0';
	/* Watch stdin (fd 0) to see when it has input. */
    FD_ZERO(&rfds);
    FD_SET(0, &rfds);

    /* Wait up to five seconds. */
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    //STDIN_FILENO-This macro has value 0, which is the file descriptor for standard input.+ 1
    retval = select(STDIN_FILENO+1, &rfds, NULL, NULL, &tv);

    if (retval == -1)
        perror("select()");
    else if (retval)
        printf("Data is available now.\n");
        /* FD_ISSET(0, &rfds) will be true. */
    else
    	//edge case
        printf("No data within five seconds.\n");

    if(retval > 0){ 
    	//reading in 1 byte and storing it in buffered_char
		read(0, &buffered_char, sizeof(buffered_char));
		return buffered_char;
	}
	return buffered_char;
}//end of the getkey method
//FINISHED

//system calls used: nano sleep
void sleep_ms(long ms){
	//http://man7.org/linux/man-pages/man2/nanosleep.2.html
	//http://timmurphy.org/2009/09/29/nanosleep-in-c-c/
	struct timespec req = {0};
	req.tv_sec = 0;
	req.tv_nsec = ms * 1000000L;
	//error handling
	if (ms < 0){
		printf("Error: a negative milisecond value was inputted. Please input a positive value.\n");
		return;
	}
	nanosleep(&req, (struct timespec *)NULL);
}//end of the sleep_ms method
//FINISHED

/*
This is the main drawing code, where the work is actually done. We want to set the pixel at coordinate (x, y) to the specified color.
You will use the given coordinates to scale the base address of the memory-mapped framebuffer using pointer arithmetic.
The frame buffer will be stored in row-major order, meaning that the first row starts at offset 
0 and then that is followed by the second row of pixels, and so on.
*/
void draw_pixel(void *img, int x, int y, color_t color){
	int offset;
	color_t* pixel;
	
	//handling errors and out of bounds exceptions
	if (x < 0){
		return;
	}
	if (y < 0){
		return;
	}
	if (x > screen_var_info.xres_virtual){
		return;
	}
	if (y > screen_var_info.yres_virtual){
		return;
	}

	offset = (y * screen_var_info.xres_virtual) + x; 
	pixel = img + offset; 
	*pixel = color; 
}//end of the draw_pixel method
//FINISHED


/*
Using draw_pixel, make a line from (x1, y1) to (x2, y2). Use Bresenham’s Algorithm 
with only integer math. Find your favorite online source to do so (and please cite the source in a comment), 
but make sure your implementation works for all valid coordinates and slopes.
used: https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm (was the best option from all avaliable links)
*/
void draw_line(void *img, int x1, int y1, int x2, int y2, color_t c){
  int dx = abs(x2-x1), sx = x1<x2 ? 1 : -1;
  int dy = abs(y2-y1), sy = y1<y2 ? 1 : -1; 
  int err = (dx>dy ? dx : -dy)/2, e2;
 
  for(;;)
  {
    draw_pixel(img, x1, y1, c);
    if (x1==x2 && y1==y2) break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x1 += sx; }
    if (e2 < dy) { err += dx; y1 += sy; }
  }
}//end of the draw_line method
//FINISHED


void *new_offscreen_buffer(){
	void *offscreen_buffer = mmap(NULL, size_of_display, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	//error handling
	if (offscreen_buffer == (void *) -1)
	{
		_exit(1); //terminate
	}
	return offscreen_buffer;
}
//FINISHED

/*
A blit is simply a memory copy from our offscreen buffer to the framebuffer. 
If we were using the C Standard Library, we’d do this using memcpy(). 
But we must write it ourselves. So, use one or two for loops and copy every byte 
from the source offscreen buffer onto the frame buffer.
*/
void blit(void *src){
	unsigned long i;
	for(i = 0; i < size_of_display; i++){
		((char *)framebuffer)[i] = ((char *)src)[i];
	}
}//end of the blit method 
//FINISHED
