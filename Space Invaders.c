#include "nios2_ctrl_reg_macros.h" 
#include "address_map.h" 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

volatile int timeout;	//global timeout varibale for synchronization 
int inc_x;	            //set it to 2 to move the enemies left two pixels at a time 
volatile int pixel_buffer_start = FPGA_ONCHIP_BASE;
int resolution_x = 320;	//max x 
int resolution_y = 240; //max y 

void clearScreen();
void drawSquare(int, int, int, int, short);
void VGA_text(int, int, char*);

typedef struct
{
	int x1;
	int x2;
	int y1;
	int y2;
	short color;
	int hit;
}ENEMY;

int main()
{
	volatile int* interval_timer_ptr = (int*)TIMER_BASE;
	volatile int* key_ptr = (int*)KEY_BASE;
	timeout = 0;

	srand(time(NULL));  

	ENEMY foes[20];
	int j, i = 0;
	int enemyx1 = 70;
	int enemyx2 = 89;
	int enemyy1 =  0;
	int enemyy2 = 19;
	int inc_x   = -2;	    
	int playx1 = 100;
	int playx2 = 119;
	int playy1 = 221;
	int playy2 = 240;
	int bulletx1, bulletx2, bullety1 = 0, bullety2 = 0;
	int enemyBulletx1, enemyBulletx2, enemyBullety1 = 250, enemyBullety2;
	int bulletActive = 0;
	short red = 0xFA00;
	short yellow = 0xFF00;
	short white = 0xFFFF;
	short black = 0x0000;
	int key_val = 0;
	int refresh = 0;
	int scoreInt = 0;
	char scoreStr[5];
	char livesStr[1];
	char levelStr[2];
	char text_erase[10] = "      \0";
	int level = 1;
	int lives = 3;
	double count = 50;
	int countIncrement = 0;
	int random;
	int gameOver = 0;
	int wait = 0;
	double temp;


	int counter = 0x930000;		//approx 200ms 
	*(interval_timer_ptr + 2) = (counter & 0xFFFF);
	*(interval_timer_ptr + 3) = (counter >> 16) & 0xFFFF;
	*(interval_timer_ptr + 1) = 0x7;

	NIOS2_WRITE_IENABLE(0x1);
	NIOS2_WRITE_STATUS(1);

	clearScreen();
	//Draw enemies initial position
	for (int row = 0; row < 4; row++)
	{
		for (int col = 0; col < 5; col++)
		{
			foes[i].x1    = enemyx1;
			foes[i].x2    = enemyx2;
			foes[i].y1    = enemyy1;
			foes[i].y2    = enemyy2;
			foes[i].color = red;
			foes[i].hit   = 1;

			drawSquare(enemyx1, enemyx2, enemyy1, enemyy2, red);
			enemyx1 += 40;
			enemyx2 += 40;
			i++;
		}
		enemyx1 -= 200;
		enemyx2 -= 200;
		enemyy1 += 40; 
		enemyy2 += 40; 
	}
	enemyy1 -= 160;
	enemyy2 -= 160;
	drawSquare(playx1, playx2, playy1, playy2, white);  //Draw Player

	while (1)
	{
		while (!timeout);	
		clearScreen();
		//Print Score and Level
		itoa(scoreInt, scoreStr, 10);
		VGA_text(315, 0, text_erase);
		VGA_text(312, 0, "SCORE: ");
		VGA_text(320, 0, "       ");
		VGA_text(320, 0, scoreStr);
		itoa(level, levelStr, 10);
		VGA_text(300, 0, "LEVEL: ");
		VGA_text(308, 0, levelStr);
		itoa(lives, livesStr, 10);
		VGA_text(290, 0, "LIVES: ");
		VGA_text(296, 0, livesStr);
		i = 0;

		//If all enemies dead, load next level
		if (refresh == 1)      
		{
			enemyx1 = 70;
			enemyx2 = 89;
			enemyy1 = 0;
			enemyy2 = 19;
			for (int row = 0; row < 4; row++)
			{
				for (int col = 0; col < 5; col++)
				{
					foes[i].x1 = enemyx1;
					foes[i].x2 = enemyx2;
					foes[i].y1 = enemyy1;
					foes[i].y2 = enemyy2;
					foes[i].color = red;
					foes[i].hit = 1;

					drawSquare(enemyx1, enemyx2, enemyy1, enemyy2, red);
					enemyx1 += 40;
					enemyx2 += 40;
					i++;
				}
				enemyx1 -= 200;
				enemyx2 -= 200;
				enemyy1 += 40;
				enemyy2 += 40;
			}
			enemyy1 -= 160;
			enemyy2 -= 160;
			i = 0;
			if (gameOver != 1)
			{
				level++;
				inc_x = -1*((abs(inc_x) + 1));
				count = 50 - (level * 7);
				if (count < 0)
					count = 1;
				countIncrement = 0;
			}
		}
		gameOver = 0;

		//Check if enemies reach edge of screen
		if ((foes[0].x1 <= 0) && (wait == 0))
		{
			inc_x = -inc_x;

			enemyy1 += 10;
			enemyy2 += 10;
			wait++;
		}
		else if ((foes[4].x2 >= 320) && (wait == 0))
		{
			inc_x = -inc_x;
			enemyy1 += 10;
			enemyy2 += 10;
			wait++;
		}
		if (wait != 0)
		{
			wait++;
			if (wait == 5)
				wait = 0;
		}

		//Animate next enemy position
		for (int row = 0; row < 4; row++)
		{
			for (int col = 0; col < 5; col++)
			{
				foes[i].x1 = enemyx1;
				foes[i].x2 = enemyx2;
				foes[i].y1 = enemyy1;
				foes[i].y2 = enemyy2;

				drawSquare(enemyx1, enemyx2, enemyy1, enemyy2, foes[i].color);
				enemyx1 += 40;
				enemyx2 += 40;
				i++;
			}
			enemyx1 -= 200;
			enemyx2 -= 200;
			enemyy1 += 40;
			enemyy2 += 40;
		}
		enemyy1 -= 160;   
		enemyy2 -= 160;   
		enemyx1 += inc_x; 
		enemyx2 += inc_x; 

		//Initialize Enemy Bullet after set amount of timer interrupts
		while (countIncrement == count)
		{
			random = (int)rand() % 20;
			if ((foes[random].hit == 1) && (enemyBullety1 >= 240))
			{
				enemyBulletx1 = foes[random].x1 + 10;
				enemyBulletx2 = enemyBulletx1 + 2;
				enemyBullety1 = foes[random].y2;
				enemyBullety2 = enemyBullety1 + 3;
				drawSquare(enemyBulletx1, enemyBulletx2, enemyBullety1, enemyBullety2, yellow);
				countIncrement = 0;
			}
			else if (enemyBullety1 < 240)
				countIncrement = 0;
		}
		countIncrement++;

		//Check if player bullet is on screen
		if (bullety2 > 0)
		{
			bullety1 -= 10;
			bullety2 -= 10;
			drawSquare(bulletx1, bulletx2, bullety1, bullety2, white);       //Draw Bullet
		}
		//Check if enemy bullet is on screen
		if (enemyBullety1 < 240)
		{
			enemyBullety1 += 10;
			enemyBullety2 += 10;
			drawSquare(enemyBulletx1, enemyBulletx2, enemyBullety1, enemyBullety2, yellow);
		}
		key_val = *(key_ptr);

		//Check value of keys
		if ((key_val == 8 || key_val == 9) && (playx1 > 0))
		{
			playx1 -= 5;
			playx2 -= 5;
		}
		if ((key_val == 2 || key_val == 3) && (playx2 < 320))
		{
			playx1 += 5;
			playx2 += 5;
		}
		if (((key_val == 1) && (bullety2 <= 0))|| ((key_val == 3) && (bullety2 <= 0)) || ((key_val == 9) && (bullety2 <= 0)))
		{
			bulletx1 = playx1 + 10;
			bulletx2 = bulletx1 + 2;
			bullety1 = 221;
			bullety2 = bullety1 + 3;
			drawSquare(bulletx1, bulletx2, bullety1, bullety2, white);       //Draw Bullet

		}

		drawSquare(playx1, playx2, playy1, playy2, white); //Draw Player

		//Check Player Bullet Collision
		for (i = 0; i < 20; i++)
		{
			if ((bulletx1 >= foes[i].x1) & (bulletx2 <= foes[i].x2) & (bullety1 >= foes[i].y1) & (bullety2 <= foes[i].y2) & (foes[i].hit == 1))
			{
				foes[i].color = black;
				foes[i].hit = 0;
				scoreInt += level;
				bullety1 = -10;
				bullety2 = -10;
			}
		}
		//Check Enemy Bullet Collision
		if ((enemyBulletx1 >= playx1) & (enemyBulletx2 <= playx2) & (enemyBullety1 >= playy1) & (enemyBullety2 <= playy2))
		{
			lives--;
			enemyBullety1 = 300;
			enemyBullety2 = 300;
		}

		//Check if enemies are all dead to reload next level
		refresh = 1;
		for (i = 0; i < 20; i++)
		{
			if (foes[i].hit == 1)
				refresh = 0;
		}	
		i = 0;

		//Check if enemies get too low (lose life)
		for (j = 0; j < 20; j++)
		{
			if ((foes[j].y2 >= 210) && (foes[j].hit == 1))
			{
				enemyx1 = 70;
				enemyx2 = 89;
				enemyy1 = 0;
				enemyy2 = 19;
				for (int row = 0; row < 4; row++)
				{
					for (int col = 0; col < 5; col++)
					{
						foes[i].x1 = enemyx1;
						foes[i].x2 = enemyx2;
						foes[i].y1 = enemyy1;
						foes[i].y2 = enemyy2;

						drawSquare(enemyx1, enemyx2, enemyy1, enemyy2, foes[i].color);
						enemyx1 += 40;
						enemyx2 += 40;
						i++;
					}
					enemyx1 -= 200;
					enemyx2 -= 200;
					enemyy1 += 40;
					enemyy2 += 40;
				}
				enemyy1 -= 160;
				enemyy2 -= 160;
				lives--;
			}
		}

		//If lives == 0 GAME OVER
		if (lives == 0)
		{
			clearScreen();
			itoa(scoreInt, scoreStr, 10);
			itoa(level, levelStr, 10);
			itoa(lives, livesStr, 10);

			while (key_val != 4)
			{
				gameOver = 1;
				VGA_text(312, 0, "SCORE: ");
				VGA_text(320, 0, scoreStr);
				VGA_text(300, 0, "LEVEL: ");
				VGA_text(308, 0, levelStr);
				VGA_text(290, 0, "LIVES: ");
				VGA_text(296, 0, livesStr);
				VGA_text(295, 30, "GAME OVER");
				VGA_text(295, 40, "Key 2 to continue");
				key_val = *(key_ptr);
			}
			scoreInt = 0;
			lives    = 3;
			level    = 1;
			refresh  = 1;
			count = 50;
			inc_x = -2;
			VGA_text(295, 30, "         ");
			VGA_text(295, 40, "                 ");
		}
		timeout = 0;  //Reset timeout for next interval interrupt 
	}
	return 0;
}

void clearScreen()
{
	int clearColor = 0;
	int pixel_ptr, row, col;

	for (row = 0; row < resolution_y; row++)
	{
		for (col = 0; col < resolution_x; col++)
		{
			pixel_ptr = pixel_buffer_start + (row << 10) + (col << 1);
			*(short*)pixel_ptr = clearColor;
		}
	}
}
void drawSquare(int x1, int x2, int y1, int y2, short color)
{
	int offset;
	int row, col; //use for the for loops 
	volatile short* pixel_buffer = (short*)0x08000000;

	for (row = y1; row <= y2; row++) //for each row 
	{
		for (col = x1; col <= x2; col++) //go through each column 
		{
			//offset y must be shifted left 10 times and x shifted once 
			offset = (row << 9) + (col);
			*(pixel_buffer + offset) = color;
		}
	}
}
void VGA_text(int x, int y, char* text_ptr)
{
	int offset;
	volatile char* character_buffer = (char*)FPGA_CHAR_BASE;

	offset = (y << 7) + x;

	while (*(text_ptr))
	{
		*(character_buffer + offset) = *(text_ptr);
		++text_ptr;
		++offset;
	}
}