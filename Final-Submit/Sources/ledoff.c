//*******************************************************************************************************
// ledoff.c
//
// Date: 2013-11-14 10:49:08
//
// DESCRIPTION:
//
// AUTHORS:
//  Abdulla Al Braiki (ayalbrai@asu.edu)
//  Zachary Priddy (zpriddy@asu.edu)
//
// Computer Science & Engineering
// Arizona State University
// Tempe, AZ 85287-8809
// 
//*******************************************************************************************************

#include "support_common.h"
#include "rng.h"
#include "oct_ledm.h"
#include "ledoff.h"
#include "oct_nunchuk.h"
#include "dtim.h"
int x, y;


int nunchuk_up;
int nunchuk_down;
int nunchuk_left ;
int nunchuk_right;
int nunchuk_z ;
int nunchuk_c;
int win_count;

static int_isr g_nunchuk_goto = 0;

oct_ledm_color basic_map[8][16] = {
		{black, black, black, black, black, black, black, black, black, black, black, black, black, black, black, black},
		{black, black, black, black, black, black, black, black, black, black, black, black, black, black, black, black},
		{black, black, black, black, black, black, black, black, black, black, black, black, black, black, black, black},
		{black, black, black, black, black, black, black, black, black, black, black, black, black, black, black, black},
		{black, black, black, black, black, black, black, black, black, black, black, black, black, black, black, black},
		{black, black, black, black, black, black, black, black, black, black, black, black, black, black, black, black},
		{black, black, black, black, black, black, black, black, black, black, black, black, black, black, black, black},
		{black, black, black, black, black, black, black, black, black, black, black, black, black, black, black, black}
};

oct_ledm_color basic_map_template[8][16] = {
		{black, black, black, black, black, black, black, black, black, black, black, black, black, black, black, black},
		{black, black, black, black, black, black, black, black, black, black, black, black, black, black, black, black},
		{black, black, black, black, black, black, black, black, black, black, black, black, black, black, black, black},
		{black, black, black, black, black, black, black, black, black, black, black, black, black, black, black, black},
		{black, black, black, black, black, black, black, black, black, black, black, black, black, black, black, black},
		{black, black, black, black, black, black, black, black, black, black, black, black, black, black, black, black},
		{black, black, black, black, black, black, black, black, black, black, black, black, black, black, black, black},
		{black, black, black, black, black, black, black, black, black, black, black, black, black, black, black, black}
};

oct_ledm_color test_map_template[8][16] = {
			{black, yellow, black, black, black, black, yellow, black, black, black, black, black, black, black, black, black},
			{yellow, yellow, yellow, black, black, yellow, yellow, yellow, black, black, black, black, black, black, black},
			{black, yellow, black, black, black, black, yellow, black, black, black, black, black, black, black, black, black},
			{black, black, black, black, black, black, black, black, black, black, black, black, black, black, black, black},
			{black, black, black, black, black, black, black, black, black, black, black, black, black, black, black, black},
			{black, yellow, black, black, black, black, yellow, black, black, black, black, black, black, black, black, black},
			{yellow, yellow, yellow, black, black, yellow, yellow, yellow, black, black, black, black, black, black, black, black},
			{black, yellow, black, black, black, black, yellow, black, black, black, black, black, black, black, black, black}
			
	};
oct_ledm_color win_pattern[8][16] = 
{
			{black, green, green, green, green, green, green, black, black, black, black, black, black, black, black, black},
			{green, black, black, black, black, black, black, green, black, black, black, black, black, black, black, black},
			{green, black, green, black, black, green, black, green, black, black, black, black, black, black, black, black},
			{green, black, black, black, black, black, black, green, black, black, black, black, black, black, black, black},
			{green, black, green, black, black, green, black, green, black, black, black, black, black, black, black, black},
			{green, black, black, green, green, black, black, green, black, black, black, black, black, black, black, black},
			{green, black, black, black, black, black, black, green, black, black, black, black, black, black, black, black},
			{black, green, green, green, green, green, green, black, black, black, black, black, black, black, black, black}
			
	};

oct_ledm_color test_map[8][16];


void set_goto(int_isr g_goto_callback)
{
	g_nunchuk_goto = g_goto_callback;
}

void start_new_game()
{
	int number_of_lights = 20;
	int light_x_y[20];
	int i, j, number, number_in_pattern;
	number_in_pattern = 0;
	
	g_nunchuk_goto = start_new_game;
	
	reset_map(&basic_map, &basic_map_template);
	
	while(5 > number_of_lights || number_of_lights > 10)
	{
		number_of_lights = (int)(rng_next())%10;
	}
	
	while(number_in_pattern < number_of_lights)
	{
		number_in_pattern = 0;
		x = (int)(rng_next())%5;
		if( x < 0 )
			x = x * -1;
		dtim0_delay_us(5000);
		y = (int)(rng_next())%5;
		if( y < 0)
			y = y * -1;
		basic_map[x][y] = yellow;
		for(i = 0 ; i < 8 ; i++)
		{
			for(j = 0 ; j < 16 ; j ++)
			{
				if(basic_map[i][j] == yellow)
				{
					number_in_pattern ++;
				}
			}
		}
	}
	
	
	
	x = 4;
	y = 4;
	basic_map[x][y] = blue;
	
	oct_ledm_display_pattern(&basic_map);
	
	set_goto(play_game);
	
	
	
	
}
void start_test_mode()
{
	reset_map(&test_map, &test_map_template);
	x = 4;
	y = 4;
	test_map[x][y] = blue;
	oct_ledm_display_pattern(&test_map);
	
	set_goto(test_mode);
}



int check_win(oct_ledm_color (*map)[8][16])
{
	int win = 1;
	int i, j;
	for(i = 0 ; i < 8 ; i++)
	{
		for(j = 0 ; j < 16 ; j ++)
		{
			if((*map)[i][j] == yellow || (*map)[i][j] == green)
			{
				win = 0;
				return win;
			}
		}
	}
	return win;
}

void reset_map(oct_ledm_color (*map)[8][16], oct_ledm_color (*reset_to)[8][16])
{
	int i, j;
	for(i = 0 ; i < 8 ; i++)
		{
			for(j = 0 ; j < 16 ; j ++)
			{
				(*map)[i][j] = (*reset_to)[i][j];
			}
		}
}

void play_game()
{
	MCF_PIT1_PCSR &= ~(MCF_PIT_PCSR_EN);
	if(nunchuk_up == 1)
			{
				if (basic_map[x][y] == green)
					basic_map[x][y] = yellow;
				else
					basic_map[x][y] = black;
				y = y;
				if(x > 0)
					x = x -1;
				else
					x = 4;
				if (basic_map[x][y] == black)
					basic_map[x][y] = blue;
				else
					basic_map[x][y] = green;
				oct_ledm_display_pattern(&basic_map);
				oct_ledm_display_pattern(&basic_map);
			}
	else if(nunchuk_down == 1)
			{
				if (basic_map[x][y] == green)
					basic_map[x][y] = yellow;
				else
					basic_map[x][y] = black;
				y = y;
				if(x < 4)
					x = x + 1;
				else 
					x = 0;
				if (basic_map[x][y] == black)
					basic_map[x][y] = blue;
				else
					basic_map[x][y] = green;
				oct_ledm_display_pattern(&basic_map);
			}
	
	else if(nunchuk_left == 1)
			{
			if (basic_map[x][y] == green)
				basic_map[x][y] = yellow;
			else
				basic_map[x][y] = black;
				x = x;
				if(y > 0)
					y = y - 1;
				else
					y = 4;
				if (basic_map[x][y] == black)
					basic_map[x][y] = blue;
				else
					basic_map[x][y] = green;
				oct_ledm_display_pattern(&basic_map);
				oct_ledm_display_pattern(&basic_map);
			}
	else if(nunchuk_right == 1)
			{
			if (basic_map[x][y] == green)
				basic_map[x][y] = yellow;
			else
				basic_map[x][y] = black;
				x = x;
				if(y < 4)
					y = y + 1;
				else 
					y = 0;
				if (basic_map[x][y] == black)
					basic_map[x][y] = blue;
				else
					basic_map[x][y] = green;
				oct_ledm_display_pattern(&basic_map);
				
			}
	
	if(nunchuk_z == 1)
	{
		if(x+1 < 5)
		{
			if(basic_map[x+1][y] == black)
				basic_map[x+1][y] = yellow;
			else
				basic_map[x+1][y] = black;
		}
		if(x-1 >= 0)
		{
			if(basic_map[x-1][y] == black)
				basic_map[x-1][y] = yellow;
			else
				basic_map[x-1][y] = black;
		}
		if(y+1 < 5)
		{
			if(basic_map[x][y+1] == black)
				basic_map[x][y+1] = yellow;
			else 
				basic_map[x][y+1] = black;
		}
		if(y-1 >= 0)
		{
			if(basic_map[x][y-1] == black)
				basic_map[x][y-1] = yellow;
			else
				basic_map[x][y-1] = black;
		}
		
		if(basic_map[x][y] == blue)
			basic_map[x][y] = green;
		else
			basic_map[x][y] = blue;
		oct_ledm_display_pattern(&basic_map);
		
		if(check_win(&basic_map) == 1)
		{
			win_count = 0;
			win_mode(1);
		}
		dtim0_delay_us(5000);
	}
	
	if(nunchuk_c == 1)
	{
		dtim0_delay_us(5000);
		g_nunchuk_goto = start_new_game;
		start_new_game();
	}
	MCF_PIT1_PCSR |= (MCF_PIT_PCSR_EN);
	
	
}

void test_mode()
{
	
	
	MCF_PIT1_PCSR &= ~(MCF_PIT_PCSR_EN);
	if(nunchuk_up == 1)
			{
				if (test_map[x][y] == green)
					test_map[x][y] = yellow;
				else
					test_map[x][y] = black;
				y = y;
				if(x > 0)
					x = x -1;
				else
					x = 7;
				if (test_map[x][y] == black)
					test_map[x][y] = blue;
				else
					test_map[x][y] = green;
				oct_ledm_display_pattern(&test_map);
			}
	else if(nunchuk_down == 1)
			{
				if (test_map[x][y] == green)
					test_map[x][y] = yellow;
				else
					test_map[x][y] = black;
				y = y;
				if(x < 7)
					x = x + 1;
				else 
					x = 0;
				if (test_map[x][y] == black)
					test_map[x][y] = blue;
				else
					test_map[x][y] = green;
				oct_ledm_display_pattern(&test_map);
			}
	
	else if(nunchuk_left == 1)
			{
			if (test_map[x][y] == green)
				test_map[x][y] = yellow;
			else
				test_map[x][y] = black;
				x = x;
				if(y > 0)
					y = y - 1;
				else
					y = 7;
				if (test_map[x][y] == black)
					test_map[x][y] = blue;
				else
					test_map[x][y] = green;
				oct_ledm_display_pattern(&test_map);
				oct_ledm_display_pattern(&test_map);
			}
	else if(nunchuk_right == 1)
			{
			if (test_map[x][y] == green)
				test_map[x][y] = yellow;
			else
				test_map[x][y] = black;
				x = x;
				if(y < 7)
					y = y + 1;
				else 
					y = 0;
				if (test_map[x][y] == black)
					test_map[x][y] = blue;
				else
					test_map[x][y] = green;
				oct_ledm_display_pattern(&test_map);
				
			}
	
	if(nunchuk_z == 1)
	{
		if(x+1 < 8)
		{
			if(test_map[x+1][y] == black)
				test_map[x+1][y] = yellow;
			else
				test_map[x+1][y] = black;
		}
		if(x-1 >= 0)
		{
			if(test_map[x-1][y] == black)
				test_map[x-1][y] = yellow;
			else
				test_map[x-1][y] = black;
		}
		if(y+1 < 8)
		{
			if(test_map[x][y+1] == black)
				test_map[x][y+1] = yellow;
			else 
				test_map[x][y+1] = black;
		}
		if(y-1 >= 0)
		{
			if(test_map[x][y-1] == black)
				test_map[x][y-1] = yellow;
			else
				test_map[x][y-1] = black;
		}
		
		if(test_map[x][y] == blue)
			test_map[x][y] = green;
		else
			test_map[x][y] = blue;
		oct_ledm_display_pattern(&test_map);
		
		if(check_win(&test_map) == 1)
		{
			win_count = 0;
			win_mode(1);	
		}
		
		dtim0_delay_us(5000);
		
	}
	MCF_PIT1_PCSR |= (MCF_PIT_PCSR_EN);
	
	
}

void win_mode(int return_to)
{
	
	if(win_count == 0)
	{
		g_nunchuk_goto = win_mode;
		oct_ledm_display_pattern(&win_pattern);
		win_count++;
	}
	else
	{
	
		if(nunchuk_z == 1)
			win_count++;
		if(win_count == 2)
		{
			
				start_new_game();
				g_nunchuk_goto = start_new_game;
			
		}
	}
	
}

void update_nunchuk_game(int r1, int r2, int r3, int r4, int r5, int c, int z)
{
	nunchuk_z = z;
	nunchuk_c = c;
	nunchuk_up = r1; 
	nunchuk_down = r2;
	nunchuk_left = r3; 
	nunchuk_right = r4;
	g_nunchuk_goto();
	
	

}