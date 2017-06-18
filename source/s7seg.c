/*
 * s7seg.c
 *
 *  Created on: Dec 29, 2012
 *      Author: dekar
 */

#include "s7seg.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

extern volatile uint16_t countdown;

const uint8_t font[18] PROGMEM =
{
		0b0000010,//0
		0b1111010,//1
		0b0001001,//2
		0b1001000,//3
		0b1110000,//4
		0b1000100,//5
		0b0000100,//6
		0b1101010,//7
		0b0000000,//8
		0b1000000,//9
		0b0100000,//A
		0b0010100,//B
		0b0000111,//C
		0b0011000,//D
		0b0000101,//E
		0b0100101,//F
		0b1111111,//
		0b1111101,//-
};
volatile uint8_t segNum;
/**************************
 * Static functions
 */

static __inline void  Q_ON(int num)
{
	if(num ==0)
	{
		PORTD |= 0b00011100;
		PORTB |= 0b01000000;
	}
	else
	{
		if(num == 4)
		{
			PORTD |= 0b00011100;
			PORTB &= ~(0b01000000);
		}
		else
		{
			PORTB |= 0b01000000;
			PORTD |= 0b00011100;
			PORTD &= ~(1 << (num+1));
		}
	}
}
static __inline void  segShow(uint8_t seg,uint8_t point)
{
	//сбрасываю сегменты
	PORTD |= 0b11100000;
	PORTB |= 0b00011111;
	seg = pgm_read_word(&font[seg]);
	if(point == 0)
		seg |= 1<<7;
	PORTD &= (seg | 0b00011111);
	PORTB &= (seg | 0b11100000);
}



/*
 * A-B4
 * F-B3
 * B-B2
 * G-B1
 * C-B0
 * P-D7
 * E-D6
 * D-D5
 *
 * Q4-B6
 * Q3-D4
 * Q2-D3
 * Q1-D2
 *
 */


/*        5
 *       ___
 *
 *     4|   |3
 *        2
 *       ---
 *
 *     7|   |1
 *
 *       ---  *8
 *        6
 */

void s7sInit(void)
{
	// Устанавливаю порты на выход
	PORTD |= 0b11111100;
	PORTB |= 0b01011111;
	DDRB |= 0b01011111;
	DDRD |= 0b11111100;
	// Включаю таймер
	TCCR0 = 0b011;//f=f/256
	TIMSK |= 1<<TOIE0;// Enable interrupt
}


ISR(TIMER0_OVF_vect)
{
	Q_ON(segNum+1);
	segShow(s7s.dat[segNum] & 0x7f,s7s.point==segNum?1:0);
	segNum = (segNum+1)&0b11;
	if(countdown)
		countdown--;
}

void s7sShowTime(struct timeT t)
{
	//старший разряд

	if(t.min<10)
	{
		s7s.dat[0] = 16;
		s7s.dat[1] = t.min;
	}
	else
	{
		s7s.dat[0] =0;
		while(t.min>9)
		{
			s7s.dat[0]++;
			t.min-=10;
		}
		s7s.dat[1] = t.min;
	}

	//младший разряд
	s7s.dat[2] =0;
	while(t.sec>9)
	{
		s7s.dat[2]++;
		t.sec-=10;
	}
	s7s.dat[3] = t.sec;
	s7s.point=1;
}
void s7sClear(void)
{
	s7s.dat[0]=16;
	s7s.dat[1]=16;
	s7s.dat[2]=16;
	s7s.dat[3]=16;
}

