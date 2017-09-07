#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>



#define ESC 0x1B



// terminal size
int height;
int width;


// 2D array, access: i*width + j
char *array;


void goto_xy(int x, int y) {
    printf("%c[%d;%df", ESC, y, x);
    fflush(stdout);
}



void read_terminal_size() {
	
	goto_xy(999, 999);
	printf("%c[6n", ESC);
	fflush(stdout);
	
    struct termios ttystate, ttysave;
	//get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);
    ttysave = ttystate;
    //turn off canonical mode and echo
    ttystate.c_lflag &= ~(ICANON | ECHO);
    //minimum of number input read.
    ttystate.c_cc[VMIN] = 1;
    
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
    
    char ch;
    int count=0;
    
    ch = getchar();
    ch = getchar();
    
    while((ch = getchar()) != 0x3B) { // 0x3B == ;
		height *= 10;
		height += (ch - 48);
	}
	
	while((ch = getchar()) != 0x52) { // 0x3B == R
		width *= 10;
		width += (ch - 48);
	}

	// restore terminal attributes
    ttystate.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &ttysave);
    
}



void init_arr() {
	for(int i = 0; i < width; i++)
		for(int j = 0; j < height; j++)
			array[i*width + j] = rand()%2 ? 33 + rand() % 93 : ' ';
}



void print_arr() {
	for(int i = 0; i < width; i++)
		for(int j = 0; j < height; j++) {
			goto_xy(i+1, j+1);
			putchar(array[i*width +j]);
		}
}



void shift_arr() {
	for(int i = 0; i < width; i++)
		for(int j = 0; j < height - 1; j++)
			 array[i*width + j] = array[i*width + (j+1)];
	
	for(int i = 0; i < width; i++)
		array[i*width + height - 1] = rand()%2 ? 33 + rand() % 93 : ' ';
}



int main() {
	
	srand(time(NULL));
	
	read_terminal_size();
	
	array = malloc(sizeof(char)*height*width);
	
	if(!array)
		return 1;
	
	init_arr();
	
	// green text
	printf("\e[32m");
	
	while(1) {
		print_arr();
		usleep(50 * 1000);
		shift_arr();
	}
	
}


