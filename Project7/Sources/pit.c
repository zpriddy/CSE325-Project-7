//*****************************************************************************
// pit.c
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
// Date: Oct 24, 2013
// 
//*****************************************************************************

#include "gpio.h"
#include "support_common.h"
#include "int.h"
#include "gpio.h"
#include "pit.h"
#include "oct_nunchuk.h"

int counter = 0; 



static int_isr g_timer0_callback = 0;
static int_isr g_timer1_callback = 0;

void pit0_init(int_isr p_callback, int p_scaler)
{
	g_timer0_callback = p_callback;
	
	int_config(55, 2, 7, pit_0_handler);

    // Disable the timer
    MCF_PIT0_PCSR &=  ~(MCF_PIT_PCSR_EN);

    // Enable the debug mode
    MCF_PIT0_PCSR |= MCF_PIT_PCSR_DBG;

    // Enable overwrite
    MCF_PIT0_PCSR |= MCF_PIT_PCSR_OVW;

    // Enable the interups
    MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIE;

    // Clear the interupt flag
    MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIF;

    // Set and forget mode (reload)
    MCF_PIT0_PCSR |= MCF_PIT_PCSR_RLD;

    // Write the prescaler
    MCF_PIT0_PCSR |= MCF_PIT_PCSR_PRE(p_scaler);

    // Write PMR0
    MCF_PIT0_PMR = (999);
}

void pit1_init() {
	 MCF_PIT1_PCSR &= ~(1); // disable timer
	                
	        MCF_PIT1_PCSR |= 1 << 5; // DEBUG MODE ENABLED
	        MCF_PIT1_PCSR |= 1 << 4; // Immediately overwrite value in PIT counter
	        MCF_PIT1_PCSR |= 1 << 3; // Enable Interrupts
	        MCF_PIT1_PCSR |= 1 << 2; // Clear interrupt request flag
	        MCF_PIT1_PCSR |= 1 << 1; // Make PCNTR0 value into PMR0
	        
	        MCF_PIT1_PMR = 249;
	        MCF_PIT1_PCSR |= (10) << 8;
	        
        
        // Interrupt Controller: PIT1 interrupts as level 2 priority 7 (Source 56)
        MCF_INTC0_ICR56 |= MCF_INTC_ICR_IL(0x02);
        MCF_INTC0_ICR56 |= MCF_INTC_ICR_IP(0x01);
        
        // Unmask interrupts from the interrupt source
        MCF_INTC0_IMRH &= ~(1 << (56 - 32));
        
        // Write PIT1 ISR address into the exception vector table (at position 64+56)
        __VECTOR_RAM[64+56] = (uint32)pit1_isr;
        
        // Enable timer
        MCF_PIT1_PCSR |= MCF_PIT_PCSR_EN;
}

void pit1_stop() {
        MCF_PIT1_PCSR &= ~(MCF_PIT_PCSR_EN);
}




void pit0_enable()
{
	// Enable the timer
	MCF_PIT0_PCSR |=  (MCF_PIT_PCSR_EN);
}

void pit0_disable()
{
	// Enable the timer
	MCF_PIT0_PCSR &=  ~(MCF_PIT_PCSR_EN);
}

void pit0_clr_flg()
{
	// Clear the interupt flag
	MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIF;
}




/*-----------------------------------------------------------------------------
 * FUNCTION: pit0_handler
 *
 * DESCRIPTION
 * ISR for pit0 handler.
 *-----------------------------------------------------------------------------*/
__declspec(interrupt) void pit_0_handler(void)
{

		//Clear Flag 
		MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIF;
		//Disable
		MCF_PIT1_PCSR &= ~(MCF_PIT_PCSR_EN);
	 	MCF_PIT0_PCSR &= ~(MCF_PIT_PCSR_PIE);
	 	
        if (g_timer0_callback)
                g_timer0_callback();
        //Enable
        MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIE;
        MCF_PIT1_PCSR |=  (MCF_PIT_PCSR_EN);

}

/*-----------------------------------------------------------------------------
 * FUNCTION: pit1_handler 
 *
 * DESCRIPTION
 * ISR for pit1 handler.
 *---------------------------------------------------------------------------*/

// Interrupt service routine for the timer
__declspec(interrupt) void pit1_isr() {
        // Clear the interrupt request
        MCF_PIT1_PCSR |= MCF_PIT_PCSR_PIF;
        
        // Disable interrupts
        MCF_PIT1_PCSR &= ~(MCF_PIT_PCSR_PIE);
        
        // Note: The display will flicker because this nunchuk_read calls a method which waits on a blocking DTIM
        // So no matter how fast this timer is, its still bottlenecked by polling DTIM in the other ISR
        if(counter == 5 )
        {
        	nunchuk_read();
        	counter = 0;
        }
        else
        	counter++;
        
        // Enable interrupts
        MCF_PIT1_PCSR |= MCF_PIT_PCSR_PIE;
}