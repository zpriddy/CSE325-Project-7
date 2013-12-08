//*******************************************************************************************************
// ledoff.h
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

#ifndef LEDOFF_H_
#define LEDOFF_H_

#include "support_common.h"
#include "pit.h"
#include "i2c.h"

extern int nunchuk_up;
void start_new_game();
void play_game();
void start_test_mode();
int check_win(oct_ledm_color (*map)[8][16]);
void win_mode(int);
void test_mode();
void reset_map(oct_ledm_color (*map)[8][16], oct_ledm_color (*reset_to)[8][16]);
void set_goto(int_isr g_goto_callback);
void update_nunchuk_game(int r1, int r2, int r3, int r4, int r5, int c, int z);

#endif /* LEDOFF_H_ */
