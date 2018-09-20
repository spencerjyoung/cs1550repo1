#include "library.c"
/*
	Spencer Young
	Peoplesoft: 4063872
	id: sjy13
	CS1550
	Disclaimer: some of the code was reused from my last semester when I took CS1550 with Dr. Farnan
*/

int main(){
	//Initialize the graphics library
	char key;
	void *buffer;

	init_graphics();

	buffer = new_offscreen_buffer();

	//Draw 1 green pixel
	draw_pixel(buffer, 200, 250, RGB(0, 60, 0)); 
	blit(buffer);
	
	sleep_ms(1000); //test the sleep function
	clear_screen(buffer);

	do {
		key = getkey();
		if(key == 'l'){
			//Draw a red line
			draw_line(buffer, 50, 50, 50, 100, RGB(60, 0, 0));
            blit(buffer);
         }
          sleep_ms(1000);
	} while (key != 'q');

	exit_graphics(); //restore default settings and exit from graphics library
	return 0;//exit
}