//*******************************************************************************************************
// rng.c
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

#include "rng.h"

uint32 value;

void rng_init()
{
	//Initializes the rng module to begin generating random numbers 
	//and pushing the random values ot the FIFO structure to be stored
	//and wait to be read
    MCF_RNGA_RNGCR |= 0x0F;
}

uint32 rng_next()
{
	//We must poll field RNGSR[OFL] to check for random data in RNGOUT
	//if there is a random value, return the value in RNGOUT
	if((MCF_RNGA_RNGSR & 0x00000100) == 0x00000100)
	{
		return MCF_RNGA_RNGOUT;
	}
}


uint32 rng_next_in_range(int low, int high)
{
	//We must keep polling field RNGSR[OFL] to check for random data in RNGOUT
	//After must keep checking if the random value in RNGOUT is between the interval [low, high]
	//Once we find a random value that falls into this interval, we return the value in RNGOUT
	while((low > value ||  value > high))
	{
		value = rng_next();
	}
	
	return value;
		
}
