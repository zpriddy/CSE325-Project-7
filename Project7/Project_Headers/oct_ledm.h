//*******************************************************************************************************
// oct_ledm.h
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

#ifndef OCT_LEDM_H_
#define OCT_LEDM_H_

/*
 -----------------------------------------------------------------------------------------------------------------------
-- Octopus Led Matrix Module (oct_ledm)
-----------------------------------------------------------------------------------------------------------------------
Define Enumerated Type oct_ledm_color
black = 0b000 = 0x00
blue = 0b001 = 0x01
green = 0b010 = 0x02
magenta = 0b101 = 0x05
orange = 0b110 = 0x06
red = 0b100 = 0x04
white = 0b111 = 0x07
yellow = 0b011 = 0x03
End Define Enumerated Type oct_ledm_color
 */

typedef enum
{
	black	= 0x00,
	blue 	= 0x01,
	green 	= 0x02,
	magenta = 0x05,
	orange	= 0x06,
	red 	= 0x04,
	white	= 0x07,
	yellow	= 0x03
	
}oct_ledm_color;

/*
 Define global variable g_ledm1_blue[8] as array of uint8
Define global variable g_ledm2_blue[8] as array of uint8
Define global variable g_ledm1_green[8] as array of uint8
Define global variable g_ledm2_green[8] as array of uint8
Define global variable g_ledm1_red[8] as array of uint8
Define global variable g_ledm2_red[8] as array of uint8
Define global variable g_row as uint8
-----------------------------------------
 */



void oct_ledm_init();
void oct_ledm_display_pattern(oct_ledm_color (*pattern)[8][16]);
void oct_ledm_set_small_pattern(oct_ledm_color small[5][5]);
void oct_ledm_refresh();
void oct_ledm_refresh_row();


#endif /* OCT_LEDM_H_ */
