//*******************************************************************************************************
// oct_nunchuk.c
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
#include "exceptions.h"
#include "dtim.h"
#include "oct_nunchuk.h"
#include "i2c.h"
#include "utils.h"
#include <stdio.h>



// Globals
static int_isr g_callback_input = 0;


void nunchuk_init() {
        g_callback_input = NULL;
        dtim3_init();
        i2c_init();
        pit1_init(); // 250ms period, calls nunchuk_read() on interrupt
        
}


void nunchuk_set_input_callback(int_isr p_callback) {
	g_callback_input = p_callback;
}

/*
 * Called every 250ms by PIT1's ISR. Reads input from the controller over I2C and sends to the appropriate handler
 */
void nunchuk_read() {
		int c_bt = 0;
		int z_bt = 0;
		int n_up = 0;
		int n_down = 0;
		int n_left = 0;
		int n_right = 0;
		uint8 state[6];
        // Don't do anything if the callback hasnt been set yet
        if(g_callback_input == NULL) {
                return;
        }
        
        // Transmit initialization commands for unencrypted mode
        nunchuk_xmit_cmd(0xF0, 0x55);
        nunchuk_xmit_cmd(0xFB, 0x00);
        nunchuk_xmit_cmd(0x00, 0x00);
        //nunchuk_xmit_cmd(0x40, 0x00); // Authentic nintendo only
        
        // Read controller state (6 bytes)
        
        i2c_rx(NUNCHUK_I2C_ADDR, 6, state, NUNCHUK_I2C_DELAY_US);
        
        // Send the individual inputs to the callback function
        
        // Left / Right joystick movement
        if(state[0] < 30) {
               // g_callback_input(NUNCHUK_INPUT_LEFT);
        		n_left = 1;
        } else if(state[0] > 200) {
        		n_right = 1;
                //g_callback_input(NUNCHUK_INPUT_RIGHT);
        }
        
        // Up / Down joystick movement
        if(state[1] < 30) {
                //g_callback_input(NUNCHUK_INPUT_DOWN);
        		n_down = 1;
        } else if(state[1] > 208) {
                //g_callback_input(NUNCHUK_INPUT_UP);
        		n_up = 1;
        }
        
        // C button
        if((state[5] & (1 << 1)) == 0) {
        	//g_callback_input();
        	//printf("BUTTON Z\n");
        	c_bt = 1;
        }
        
        // Z button
        if((state[5] & (1 << 0)) == 0) {
        	//g_callback_input();
        	z_bt = 1;
        }
        
        g_callback_input(n_up,n_down,n_left,n_right,state[5],c_bt,z_bt);
        
        
}


void nunchuk_xmit_cmd(uint8 reg, uint8 cmd) {
        uint8 data[2];
        int size = 0;
        if(reg != 0x00) { // Reg and command sent to nunchuck register
                size = 2;
                
                data[0] = reg;
                data[1] = cmd;
        } else { // Command send to a nunchuk register
                size = 1;
                
                data[0] = cmd;
        }
        
        i2c_tx(NUNCHUK_I2C_ADDR, size, data, NUNCHUK_I2C_DELAY_US);
        
        dtim3_delay_us(2 * NUNCHUK_I2C_DELAY_US);
       
}