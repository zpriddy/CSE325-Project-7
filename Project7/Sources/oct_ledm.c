//*******************************************************************************************************
// oct_ledm.c
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
#include "oct_ledm.h"
#include "qspi.h"
#include "pit.h"
#include "dtim.h"


uint8 led_current_pattern[8][8];
uint8 g_green[8];
uint8 g_red[8];
uint8 g_ledm1_blue[8];
uint8 g_ledm2_blue[8];
uint8 g_ledm1_green[8];
uint8 g_ledm2_green[8];
uint8 g_ledm1_red[8];
uint8 g_ledm2_red[8];
uint8 g_row;


oct_ledm_color test[8][16] = {
			{black, yellow, black, black, black, black, yellow, black, black, green, green, green, green, green, green, black},
			{yellow, yellow, yellow, black, black, yellow, yellow, yellow, green, black, black, black, black, black, black, green},
			{black, yellow, black, black, black, black, yellow, black, green, black, green, black, black, green, black, green},
			{black, black, black, black, black, black, black, black, green, black, black, black, black, black, black, green},
			{black, black, black, black, black, black, black, black, green, black, green, black, black, green, black, green},
			{black, yellow, black, black, black, black, yellow, black, green, black, black, green, green, black, black, green},
			{yellow, yellow, yellow, black, black, yellow, yellow, yellow, green, black, black, black, black, black, black, green},
			{black, yellow, black, black, black, black, yellow, black, black, green, green, green, green, green, green, black}
			
	};
/*

/*
 -----------------------------------------------------------------------------------------------------------------------
-- oct__ledm_init: Initializes the hardware so we can use the Octopus board LED matrices.
-- Note: Do not enable PIT0 yet. It is started when oct_ledm_display_pattern is called.
-----------------------------------------------------------------------------------------------------------------------
Function oct_ledm_init () Returns Nothing
Configure GPIO port TI pin 5 for the quaternary (GPIO) function -- LEDM_STR
Configure GPIO port TI pin 3 for the quaternary (GPIO) function -- LEDM_OE
Deassert LEDM_STR
Deassert LEDM_OE
Call qspi_init (qspi_8_data_bits, 5 MHz, qspi_cpol_0, qspi_cpha_0)
Configure PIT0 to generate periodic interrupts at 500 Hz; the ISR shall call oct_ledm_refresh() at every interrupt
End Function oct_ledm_init
------------------------------
 */
void oct_ledm_init()
{
	MCF_GPIO_PTIPAR &= ~(MCF_GPIO_PTIPAR_PTIPAR5); // GPIO TI pin 5 for GPIO function, LEDM_STR
	MCF_GPIO_PTIPAR &= ~(MCF_GPIO_PTIPAR_PTIPAR3); // GPIO TI pin 3 for GPIO function, LEDM_OE
	
	
	MCF_GPIO_DDRTI |= MCF_GPIO_DDRTI_DDRTI5 | MCF_GPIO_DDRTI_DDRTI3;
	
	MCF_GPIO_SETTI &= MCF_GPIO_SETTI_SETTI5;
	MCF_GPIO_SETTI |= MCF_GPIO_SETTI_SETTI3;
	
	qspi_init(5000, 0);
	
	pit0_init(oct_ledm_refresh, 0x01);
	
	g_row = 0;
	
}

/*
 -----------------------------------------------------------------------------------------------------------------------
-- oct_ledm_refresh: Called at 500 Hz by the PIT0 ISR. Refreshes the LED matrix by refreshing the next row in sequence.
-----------------------------------------------------------------------------------------------------------------------
Function oct_ledm_refresh () Returns Nothing
Call oct_ledm_refresh_row() to turn on the LED's in g_row
Increment g_row mod 8
End Function oct_ledm_refresh
--------------------------------
 */
void oct_ledm_refresh()
{
	
		oct_ledm_refresh_row();
	
		g_row++;
		if(g_row == 8)
		{
			g_row = 0;
			MCF_PIT1_PCSR |=  (MCF_PIT_PCSR_EN);
		}
		else
		{
			MCF_PIT1_PCSR &=  ~(MCF_PIT_PCSR_EN);
		}
	

}

/*
Function oct_ledm_display_pattern (Input: pattern[8][16] as oct_ledm_color) Returns Nothing
Stop PIT0
For row < 0 to 7 Do
Initialize g_ledm1_red[row] < g_ledm1_green[row] < g_ledm1_blue[row] < 0x00
Initialize g_ledm2_red[row] < g_ledm2_green[row] < g_ledm2_blue[row] < 0x00
For col < 0 to 7 Do
If pattern[row][col] contains a red component Then set bit col of g_ledm2_red[row]
If pattern[row][col] contains a green component Then set bit col of g_ledm2_green[row]
If pattern[row][col] contains a blue component Then set bit col of g_ledm2_blue[row]
If pattern[row][col + 8] contains a red component Then set bit col of g_ledm1_red[row]
If pattern[row][col + 8] contains a green component Then set bit col of g_ledm1_green[row]
If pattern[row][col + 8] contains a blue component Then set bit col of g_ledm1_blue[row]
End For col
End For row
g_row < 0
Start PIT0
End Function oct_ledm_display_pattern
 */
void oct_ledm_all_off()
{
	int row;

	for(row = 0 ; row < 8 ; row++)
		{
			g_ledm1_red[row] = g_ledm1_green[row] = g_ledm1_blue[row] = (uint8)0xFF;
			g_ledm2_red[row] = g_ledm2_green[row] = g_ledm2_blue[row] = (uint8)0xFF;
		}
	
}

void oct_ledm_display_pattern(oct_ledm_color (*tests)[8][16])
{
	 oct_ledm_color (*pattern)[8][16] = tests;
	 
	int row, col;
	g_row=0;
	
	
	 pit0_disable();
	
	
	for(row = 0 ; row < 8 ; row++)
	{
		g_ledm1_red[row] = g_ledm1_green[row] = g_ledm1_blue[row] = (uint8)0x0000;
		g_ledm2_red[row] = g_ledm2_green[row] = g_ledm2_blue[row] = (uint8)0x0000;
	}
	
	
	
	for(row = 0 ; row < 8 ; row++)
	{
		for(col = 0 ; col < 8 ; col++)
		{
			
			
			switch((*pattern)[row][col])
			{
				case blue:
					g_ledm2_blue[row] |= (uint8)(1 << col);
					//g_ledm2_green[row] &= ~(uint8)(0x1 << col);
					//g_ledm2_red[row] &= ~(uint8)(0x1 << col);
					break;
				case green:
					g_ledm2_green[row] |= (uint8)(0x1 << col);
					//g_ledm2_blue[row] &= ~(uint8)(0x1 << col);
					//g_ledm2_red[row] &= ~(uint8)(0x1 << col);
					break;
				case red:
					g_ledm2_red[row] |= (uint8)(0x1 << col);
					//g_ledm2_green[row] &= ~(uint8)(0x1 << col);
					//g_ledm2_blue[row] &= ~(uint8)(0x1 << col);
					break;
				case orange:
					//g_ledm2_red[row] |= (uint8)(0x1 << col);
					g_ledm2_green[row] |= (uint8)(0x1 << col);
					g_ledm2_blue[row] |= (uint8)(0x1 << col);
					break;
				case magenta:
					g_ledm2_blue[row] |= (uint8)(0x1 << col);
					//g_ledm2_green[row] &= ~(uint8)(0x1 << col);
					g_ledm2_red[row] |= (uint8)(0x1 << col);
					break;
				case yellow:
					//g_ledm2_blue[row] |= (uint8)(0x1 << col);
					g_ledm2_green[row] |= (uint8)(0x1 << col);
					g_ledm2_red[row] |= (uint8)(0x1 << col);
					break;
				case white:
					g_ledm2_red[row] |= (uint8)(0x1 << col);
					g_ledm2_green[row] |= (uint8)(0x1 << col);
					g_ledm2_blue[row] |= (uint8)(0x1 << col);
					break;
				
			};
			
			switch((*pattern)[row][col+8])
			{
				case orange:
					g_ledm1_green[row] |= (uint8)(0x1 << col);
					g_ledm1_blue[row] |= (uint8)(0x1 << col);
					break;
				case green:
					g_ledm1_green[row] |= (uint8)(0x1 << col);
					//g_ledm2_blue[row] &= ~(uint8)(0x1 << col);
					//g_ledm2_red[row] &= ~(uint8)(0x1 << col);
					break;
				case red:
					g_ledm1_red[row] |= (uint8)(0x1 << col);
					//g_ledm2_green[row] &= ~(uint8)(0x1 << col);
					//g_ledm2_blue[row] &= ~(uint8)(0x1 << col);
					break;
				
				case magenta:
					g_ledm1_blue[row] |= (uint8)(0x1 << col);
					//g_ledm1_green[row] |= (uint8)(0x1 << col);
					g_ledm1_red[row] |= (uint8)(0x1 << col);
					break;
				case yellow:
					//g_ledm1_blue[row] |= (uint8)(0xF << col);
					g_ledm1_green[row] |= (uint8)(0x1 << col);
					g_ledm1_red[row] |= (uint8)(0x1 << col);
					break;
				case white:
					g_ledm1_red[row] |= (uint8)(0x1 << col);
					g_ledm1_green[row] |= (uint8)(0x1 << col);
					g_ledm1_blue[row] |= (uint8)(0x1 << col);
					break;
				case blue:
					g_ledm1_blue[row] |= (uint8)(0x1 << col);
					break;

			};
			
			
		}
		
	
	}
	
	
	
	
	pit0_enable();
	
}





/*
-----------------------------------------------------------------------------------------------------------------------
-- oct_ledm_refresh_row: Called at 500 Hz by oct_ledm_refresh().
-----------------------------------------------------------------------------------------------------------------------
Function oct_ledm_refresh_row () Returns Nothing
Define sdout[8] as array of uint8
sdout[0] < g_ledm2_green -- 1st transmitted byte is green byte of row g_row for LEDM2
sdout[1] < g_ledm2_red -- 2nd transmitted byte is red byte of row g_row for LEDM2
sdout[2] < g_ledm2_blue -- 3rd transmitted byte is blue byte of row g_row for LEDM2
sdout[3] < ~(1 << g_row) -- 4th transmitted byte is row select for row g_row
sdout[4] < g_ledm1_green -- 5th transmitted byte is green byte of row g_row for LEDM1
sdout[5] < g_ledm1_red -- 6th transmitted byte is red byte of row g_row for LEDM1
sdout[6] < g_ledm1_blue -- 7th transmitted byte is blue byte of row g_row for LEDM1
sdout[7] < sdout[3] -- 8th transmitted byte is row select for row g_row
 Deassert LEDM_OE -- Disable the outputs of the 74HC595's and the TLC5916's.
Deassert LEDM_S -- Will bring high later to transfer bits in shift registers to storage registers.
qspi_tx(8, sdout) -- Transfer 8-bytes (64-bits)
Assert LEDM_STR -- Low-to-high transition on LEDM_STR will move bits to storage registers.
Assert LEDM_OE -- When asserted, contents of storage registers are output to LED matrices
End Function oct_ledm_refresh_row
*/


void oct_ledm_refresh_row()
{
	
	
	uint8 sdout[8];
	
	sdout[0] = 0x00;;
	sdout[1] = 0x00;
	sdout[2] = 0x00;
	sdout[3] = ~(1 << g_row);
	sdout[4] = 0x00;
	sdout[5] = 0x00;
	sdout[6] = 0x00;
	sdout[7] = sdout[3];
	
	MCF_GPIO_SETTI |= MCF_GPIO_SETTI_SETTI5;
		MCF_GPIO_CLRTI &= ~(MCF_GPIO_SETTI_SETTI3);
		
		
	

	qspi_tx(sdout, 8);
	
	
	MCF_GPIO_SETTI |= MCF_GPIO_SETTI_SETTI3;
		MCF_GPIO_CLRTI &= ~(MCF_GPIO_SETTI_SETTI5);
		
		sdout[0] = g_ledm2_green[g_row];
		sdout[1] = g_ledm2_red[g_row];
		sdout[2] = g_ledm2_blue[g_row];
		sdout[3] = ~(1 << g_row);
		sdout[4] = g_ledm1_green[g_row];
		sdout[5] = g_ledm1_red[g_row];
		sdout[6] = g_ledm1_blue[g_row];
		sdout[7] = sdout[3];
		
		MCF_GPIO_SETTI |= MCF_GPIO_SETTI_SETTI5;
			MCF_GPIO_CLRTI &= ~(MCF_GPIO_SETTI_SETTI3);
			
			
		

		qspi_tx(sdout, 8);
		
		
		MCF_GPIO_SETTI |= MCF_GPIO_SETTI_SETTI3;
			MCF_GPIO_CLRTI &= ~(MCF_GPIO_SETTI_SETTI5);	
	
	
	
	
}

















