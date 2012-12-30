/*
 * chrgPmp.c
 *
 *  Created on: Dec 29, 2012
 *      Author: dekar
 */

#include "chrgPmp.h"
#include <avr/io.h>
#include <avr/delay.h>

/*
 * ON/OFF - C4
 * CHRG - C5
 * (1 - PUSH, 0 - WRK)
 */
#define ON_OFF PORT4
#define CHRG PORT5

#define SW_ON PORTC |= (1<<ON_OFF)
#define SW_OFF PORTC &= ~(1<<ON_OFF)

#define PMP_START PORTC |= (1<<CHRG)
#define PMP_OPERATE PORTC &= ~(1<<CHRG)

void my_delay2(uint16_t ms)
{
	while(ms >0)
	{
		ms--;
		_delay_ms(1);
	}
}

void chrgPmpInit(void)
{
	SW_ON;
	PMP_OPERATE;
	DDRC |= 0b00110000;
}

void chrgStrat(void)
{
	PMP_START;
	my_delay2(100);
	PMP_OPERATE;
}

void chrgStop(void)
{
	SW_OFF;
	my_delay2(100);
	SW_ON;
}
