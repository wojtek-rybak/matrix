#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>



#define ESC 0x1B



// terminal size
int height;
int width;

char *array; // 2D array, access: i*width + j
int *speed;



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



void init() {
	for(int i = 0; i < height; i++)
		for(int j = 0; j < width; j++)
			array[i*width + j] = rand()%2 ? 33 + rand() % 93 : ' ';
	
	for(int i = 0; i < width; i++) {
		int rand_int = rand() % 8;
		if(rand_int < 3) {
			speed[i] = 1;
		} else if(rand_int == 7) {
			speed[i] = 3;
		} else {
			speed[i] = 2;
		}
	}
}



void print() {
	for(int i = 0; i < height; i++)
		for(int j = 0; j < width; j++) {
			goto_xy(j+1, i+1);
			putchar(array[i*width + j]);
		}
}



void shift() {
	for(int i = 0; i < height; i++)
		for(int j = 0; j < width; j++)
			if(i + speed[j] < height)
				array[i*width + j] = array[(i+speed[j])*width + j];
			else
				array[i*width + j] = rand()%2 ? 33 + rand() % 93 : ' ';
	
	for(int j = 0; j < width; j++)
		array[(height - 1)*width + j] = rand()%2 ? 33 + rand() % 92 : ' ';
}



int main() {
	
	srand(time(NULL));
	
	read_terminal_size();
	
	array = malloc(sizeof(char)*height*width);
	speed = malloc(sizeof(int)*width);
	
	if(!array || !speed)
		return 1;
	
	init();
	
	// green text
	printf("\e[32m");
	
	while(1) {
		print();
		usleep(50 * 1000);
		shift();
	}
	
}


