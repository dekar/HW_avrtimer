/*
 * platform.h
 *
 *  Created on: Jun 2, 2012
 *      Author: dekar
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_
#include <stdint.h>

struct _t_button
{
	uint8_t oldState;
	uint8_t timeout;
};
extern struct _t_button button;
struct timeT
{
	uint8_t min;
	uint8_t sec;
};

/*#define PIN_DATA 	(1 << PORT0)
#define PIN_CK 		(1 << PORT1)
#define PIN_SCL 	(1 << PORT2) //CS
#define PIN_RCK 	(1 << PORT3) // Строб
#define PIN_OE 		(1 << PORT4)
#define PORT_SHR 	PORTD
#define DDR_SHR 	DDRD

#define TRANS_INIT DDRD |= 0b11100000; PORTD &= 0b00011111
#define TRANS_ON(a) PORTD |= 1 << (a+5)
#define TRANS_OFF(a) PORTD &= ~(1 << (a+5))

#define KEY_PATTERN 0b10010101*/

#endif /* PLATFORM_H_ */
