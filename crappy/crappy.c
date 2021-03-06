#include <string.h> // memset
#include <stdint.h>
#include <stdlib.h> // rand
#include "bitbox.h"


// global state
int game_title;
int y, vy;
int score;

// barres
int x, h1, h2; // X du premier element (les deux sont espacés de la 320) 
uint16_t c1, c2; // couleur barres
int but_state, but_last;


// const data

// All vertical values are *16
const int JUMP_SPEED = 64;
const int gravity=3;
const uint16_t BGCOLOR = RGB(100,100,255);
const int hspeed = 4; // pixels per frame

const int col_width=64;
const int col_height=100;

const uint16_t score_color = RGB(255,255,0);
const int score_y = 4;
const int score_h = 4;

// sprite data
const int oiseau_h = 23; // XXX change me
const int oiseau_w = 32;

const unsigned char *oiseau_data[2]= {
(const unsigned char *)&(const unsigned char[]){0x55,0x55,0x55,0x55,0x55,0x00,0x01,0x55,0x55,0x55,0x55,0x55,0x15,0xff,0xfc,0x55,0x51,0x01,0x50,0x55,0x00,0xff,0xfc,0x54,0x48,0xa0,0x4a,0x15,0x28,0xff,0xf0,0x54,0x48,0xa8,0x4a,0x15,0x2a,0xff,0xc0,0x54,0x48,0xa8,0x2a,0x05,0x2a,0x0f,0x00,0x40,0x28,0xa8,0x2a,0x85,0x2a,0x03,0xaa,0x2a,0x28,0xa0,0xaa,0x80,0x2a,0x03,0x0a,0x00,0xa8,0xa2,0xaa,0x88,0xaa,0x80,0xaa,0x2a,0xa8,0xa2,0xaa,0x88,0xaa,0xaa,0x00,0x00,0xa8,0xa2,0xaa,0x8a,0xaa,0xaa,0x2a,0x55,0xa8,0xa2,0xaa,0x8a,0xaa,0x0a,0x40,0x55,0xa8,0x82,0xaa,0xaa,0xaa,0x52,0x55,0x55,0xa8,0x0a,0x80,0xaa,0xaa,0x52,0x55,0x55,0xa1,0xaa,0xaa,0xaa,0xaa,0x52,0x55,0x55,0x85,0xaa,0xaa,0xaa,0xaa,0x54,0x55,0x55,0x15,0xaa,0xaa,0xaa,0x2a,0x55,0x55,0x55,0x55,0xa8,0xaa,0xaa,0x4a,0x55,0x55,0x55,0x55,0xa1,0xaa,0xaa,0x50,0x55,0x55,0x55,0x55,0x05,0xa8,0x02,0x55,0x55,0x55,0x55,0x55,0x55,0x01,0x54,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x5},
(const unsigned char *)&(const unsigned char[]){0x55,0x55,0x55,0x55,0x55,0x00,0x01,0x55,0x55,0x55,0x55,0x55,0x15,0x0f,0xc0,0x55,0x51,0x55,0x55,0x55,0x00,0x0f,0xc0,0x54,0x48,0x55,0x55,0x15,0x28,0xff,0xfc,0x54,0x48,0x55,0x55,0x15,0x2a,0xff,0xfc,0x54,0x48,0x55,0x55,0x05,0x2a,0xff,0x00,0x54,0x28,0x55,0x55,0x85,0x2a,0x3f,0x2a,0x40,0x28,0x55,0x55,0x81,0x2a,0x3f,0xaa,0x2a,0xa8,0x00,0x00,0x88,0xaa,0x80,0xaa,0x2a,0xa8,0xaa,0xaa,0x8a,0xaa,0xaa,0x02,0x40,0xa8,0xa8,0xaa,0x8a,0xaa,0xaa,0x52,0x55,0x28,0xaa,0xaa,0x8a,0xaa,0xaa,0x4a,0x55,0x28,0xaa,0xaa,0xaa,0xaa,0x2a,0xaa,0x54,0x88,0xaa,0xaa,0xa2,0xaa,0x2a,0xa8,0x54,0x88,0xaa,0xaa,0xa2,0xaa,0x42,0xa1,0x52,0x88,0xaa,0xaa,0xa2,0xaa,0x54,0x05,0x50,0xa0,0xaa,0xaa,0xa2,0x2a,0x55,0x55,0x55,0xa0,0xaa,0xaa,0xa2,0x4a,0x55,0x55,0x55,0xa1,0xaa,0xaa,0xa2,0x50,0x55,0x55,0x55,0xa1,0xaa,0xaa,0x00,0x55,0x55,0x55,0x55,0xa1,0xaa,0x2a,0x54,0x55,0x55,0x55,0x55,0xa1,0x0a,0x40,0x55,0x55,0x55,0x55,0x55,0x05,0x50,0x55,0x55,0x55,0x55,0x55,0x5},
};

const uint16_t palette[]={0,0,RGB(255,255,0),0xffff}; // 3 non transp colors

int randint(int min, int max) {
	return min + rand()%(max-min);
}

void new_game()
{
	score=0;

	// reset bars 
	x = 230;

	h1 = randint(150,350);	
	h2 = randint(150,350);
	c1 = randint (0,32768);
	c2 = randint (0,32768);

	// reset bird
	y = 240*16;
	vy = -JUMP_SPEED;

	// start
	game_title = 0;
}
void game_init() {}

void game_frame()
{
	// detect button press (ie change of state)
	but_state = button_state() || GAMEPAD_PRESSED(0,A);
	if (!but_last && but_state) {
		if (game_title) 
			new_game();
		else
			vy = -JUMP_SPEED; // jump
	}
	but_last = but_state;

	// if showing title, dont change the state
	if (game_title) return;

	vy+=gravity; 
	y+=vy;


	// move columns
	x -= hspeed + score/10; 
	if (x<=-col_width) {
		c1=c2; c2=randint (0,32768);
		h1=h2; h2=randint(150,350);
		x+=640/2;
		score+=1; 
	}
	

	// collision test with second column : return to title
	
	if ((320+oiseau_h/2 >= (x+320) && 320-oiseau_h/2 <= x+320+col_width) && ( (y/16) <= h2 || (y/16)+oiseau_h >= h2+col_height ))
			game_title=1;

	if (y/16>480-oiseau_h || y<=0) game_title=1;

	// output sound ?
}

// my own graphical engine
void graph_frame() {}
void graph_line()
{
	// draw background as plain color => degrade ? 
	for (int i=0;i<640;i++) draw_buffer[i] = BGCOLOR + (vga_line/32)*0b10000100000;
	
	// draw columns as color bars
	if (vga_line<h1 || vga_line>h1+col_height)
		memset(&draw_buffer[x<0?0:x],c1,(x>0?col_width:col_width+x)*sizeof(uint16_t)); // ne pas depasser buffer a gauche
	if (vga_line<h2 || vga_line>h2+col_height)
		memset(&draw_buffer[x+640/2],c2,col_width*sizeof(uint16_t)); // peut depasser a droite, les buffers sont plus grands

	// blit bird 4 color sprite
	uint16_t *dest = &draw_buffer[(640-oiseau_w)/2]; 
	if (vga_line>=(y/16) && vga_line<(y/16)+oiseau_h) {
		for (int i=0;i<8;i++) // bird line is 8 bytes of 4 pixels each
		{
			uint8_t b = oiseau_data[vy>0?0:1][(vga_line-(y/16))*8 + i]; 
			// blit 4 u16 pixels
			for (int j=0;j<4;j++ , b>>=2)
				if((b&3) != 1) 
					*dest++ = palette[b&3];
				else
					dest++;
		}
	}

	// draw score
	if (vga_line>score_y && vga_line<=score_y+score_h)
		for (int i=0;i<score;i++)
			draw_buffer[32+2*i] = score_color;
}

void game_snd_buffer(uint16_t *buffer, int len) {}