//*******************************************************************************************************
// i2c.h
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

#include "i2c.h"
#include "int.h"
#include "gpio.h"
#include "dtim.h"
#include "support_common.h"
#include "utils.h"

void i2c_init() {
		uint8 dummy;
        // Initialize DMA timer 
        dtim0_init();
        
        
        MCF_GPIO_PUBPAR |= MCF_GPIO_PUBPAR_UTXD1_SCL1;
        MCF_GPIO_PUBPAR |= MCF_GPIO_PUBPAR_URXD1_SDA1;
        
        
        // Write the I2C board address
        MCF_I2C1_I2ADR |= MCF_I2C_I2ADR_ADR(I2C_ADDR);
        
        // Write the i2c clock rate. f_i2c = 80Mhz / divider
        // divider = 80Mhz / 0.1 Mhz = 800. Closest divider in Table 29.2 896 for 89.29Khz, IC = 0x3A
        MCF_I2C1_I2FDR |= MCF_I2C_I2FDR_IC(0x3A);
        
        // Reset to slave-receiver mode
        i2c_reset();
        

        if(MCF_I2C1_I2SR & MCF_I2C_I2SR_IBB) {
                MCF_I2C1_I2CR = 0x00; // Board is slave-receiver, i2c disabled, interrupts disabled
                MCF_I2C1_I2CR = 0xA0; // Board is master-receiver, i2c enabled, interrupts, disabled
                dummy = MCF_I2C0_I2DR; // Dummy read from slave which is transmitting
                MCF_I2C1_I2SR = 0x00; // Clear arbitration lost flag, clear i2c interrupt request flag
                MCF_I2C1_I2CR = 0x00; // Board is slave-receiver, i2c disabled, interrupts disabled
                MCF_I2C1_I2CR = 0x80; // Enable I2C module
        }
}


void i2c_acquire_bus() {
        // Loop until I2C bus busy (IBB) becomes 0
        while(MCF_I2C1_I2SR & MCF_I2C_I2SR_IBB) {
        }
}


void i2c_reset() {
        MCF_I2C1_I2CR |= MCF_I2C_I2CR_IEN; // Enable i2c module
        MCF_I2C1_I2CR &= ~(MCF_I2C_I2CR_IIEN); // Disable interrupts, we use polling instead
        MCF_I2C1_I2CR &= ~(MCF_I2C_I2CR_MSTA); // Make board a slave
        MCF_I2C1_I2CR &= ~(MCF_I2C_I2CR_MTX); // Make board a receiver
        MCF_I2C1_I2CR &= ~(MCF_I2C_I2CR_TXAK); // Automatically ACK recv'd bytes
        MCF_I2C1_I2CR &= ~(MCF_I2C_I2CR_RSTA); // Don't generate repeated start bits
}


void i2c_rx(uint8 addr, int size, uint8 *data, int delay_us) {
		uint8 dummy;
		int i;
        i2c_acquire_bus(); // Wait for bus to become idle
        i2c_tx_addr(addr, I2C_READ, delay_us); // Send start bit, slave address, and read bit

        MCF_I2C1_I2CR &= ~(MCF_I2C_I2CR_MTX); // Become a receiver
        MCF_I2C1_I2CR &= ~(MCF_I2C_I2CR_TXAK); // Configure to ACK each recv'd data byte
        
        // Read a dummy byte in order to complete the mode switch
        dummy = i2c_rx_byte(delay_us);
        
        // Master-receivers must generate clock pulses on SCL to recv 8 data bytes from slave
        // Read and ACK up until the last byte (size-2)
        for( i = 0; i <= (size-2); i++)
                data[i] = i2c_rx_byte(delay_us);
        
        // NACK to stop transmission and read the last byte
        MCF_I2C1_I2CR |= MCF_I2C_I2CR_TXAK;
        data[size-1] = i2c_rx_byte(delay_us);
        
        // Terminate communication
        i2c_rxtx_end();
}


uint8 i2c_rx_byte(int delay_us) {
        uint8 ret = MCF_I2C1_I2DR; // Read, which generates SCL for the slave to transmit
        
        // Wait for transfer to finish
        while(!i2c_tx_complete()) {
        }
        MCF_I2C1_I2SR &= ~(MCF_I2C_I2SR_IIF); // Clear interrupt request flag
        
        // Delay for delay_us following the transfer
        dtim0_delay_us(delay_us);
        return ret;
}


void i2c_rxtx_end() {
        MCF_I2C1_I2CR &= ~(MCF_I2C_I2CR_MSTA); // Make board a slave
        i2c_reset();
}


void i2c_tx(uint8 addr, int size, uint8 *data, int delay_us) {
	int i;
        i2c_acquire_bus(); // Waits for bus to become idle
        i2c_tx_addr(addr, I2C_WRITE, delay_us); // Transmit start bit, slave address, and write bit
        
        // Send each byte in data
        for( i = 0; i < size; i++)
                i2c_tx_byte(data[i], delay_us);
        
        // Terminate communication with slave
        i2c_rxtx_end();
}


void i2c_tx_addr(uint8 addr, uint8 rw, int delay_us) {
		uint8 hello = 0;
        MCF_I2C1_I2CR |= MCF_I2C_I2CR_MTX; // Make board a transmitter
        MCF_I2C1_I2CR |= MCF_I2C_I2CR_MSTA; // Make board a master (which sends the start bit)
        
        // Compound first 7 address bits followed by the rw bit, then send
        //uint8 hello = 0;
        hello |= addr << 1;
        hello |= rw << 0;
        i2c_tx_byte(hello, delay_us);
}


void i2c_tx_byte(uint8 data, int delay_us) {
        //asm_set_ipl(7); // Mask all interrupt levels !!! necessary?
        
        // Write data
        MCF_I2C1_I2DR = data;
        
        // Wait for data to finish transmitting
        while(!i2c_tx_complete()) {
        }
        MCF_I2C1_I2SR &= ~(MCF_I2C_I2SR_IIF); // Clear interrupt request flag
        
        //asm_set_ipl(0); // Unmask all interrupt levels !!! necessary?
        
        // Delay for delay_us following the transfer
        dtim0_delay_us(delay_us);
}


int i2c_tx_complete() {
        return (MCF_I2C1_I2SR & MCF_I2C_I2SR_IIF);
}