/*
 * s7seg.h
 *
 *  Created on: Dec 29, 2012
 *      Author: dekar
 */

#ifndef S7SEG_H_
#define S7SEG_H_
#include "platform.h"
#include <stdint.h>


struct
{
	uint8_t dat[4];
	uint8_t point;
} s7s;


void s7sInit(void);
void s7sPool(void);
void s7sShowTime(struct timeT t);
void s7sClear(void);



#endif /* S7SEG_H_ */
