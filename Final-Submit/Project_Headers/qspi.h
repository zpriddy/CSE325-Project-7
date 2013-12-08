/*
 * qspi.h
 *
 *  Created on: Dec 5, 2013
 *      Author: zpriddy
 */

#ifndef QSPI_H_
#define QSPI_H_

#include <stdio.h>

#include "support_common.h"

// Imported globals
extern uint32 __VECTOR_RAM[];

// Globals
extern uint16 g_cmd;

void qspi_init(int baud, int delay);
void qspi_tx(uint8 *data, unsigned short size);

#endif /* QSPI_H_ */
