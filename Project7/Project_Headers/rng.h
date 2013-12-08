//*******************************************************************************************************
// rng.h
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

#ifndef RNG_H
#define RNG_H

//include all necessary files
#include "MCF52259.h"

/*--------------------------------------------------------------------------------------------------------------
* FUNCTION: rng_init()
*
* DESCRIPTION
* initializes the ColdFire Random Number Generator (RNG) module
*-------------------------------------------------------------------------------------------------------------*/
void rng_init();

/*--------------------------------------------------------------------------------------------------------------
* FUNCTION: rng_next()
*
* DESCRIPTION
* returns an unsigned 32-bit random integer.
*-------------------------------------------------------------------------------------------------------------*/
uint32 rng_next();

/*--------------------------------------------------------------------------------------------------------------
* FUNCTION: rng_next_in_range()
*
* DESCRIPTION
* returns an unsigned random integer in [low, high]
*-------------------------------------------------------------------------------------------------------------*/
uint32 rng_next_in_range();

#endif