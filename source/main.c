/*
 * main.c
 *
 *  Created on: Jun 2, 2012
 *      Author: dekar
 */
#include <inttypes.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "platform.h"
#include "stdint.h"
#include "s7seg.h"
#include "chrgPmp.h"
struct _t_button button;

uint16_t EE_timerMin EEMEM = 0x0003;
uint16_t EE_timerMax EEMEM  = 0x0005;
uint16_t EE_Calibr EEMEM  = 478;

volatile uint16_t countdown;
static struct timeT timeout;
static uint16_t calibr;
void my_delay(uint16_t ms)
{
	while(ms >0)
	{
		ms--;
		_delay_ms(1);
	}
}
struct
{
	enum
	{
		IDLE,
		RUN,
		SET
	}state;
}loop;

uint8_t timeoutDecriment()
{
	if(timeout.sec>0)
	{
		timeout.sec--;
		s7sShowTime(timeout);
	}
	else
	{
		if(timeout.min>0)
		{
			timeout.min--;
			timeout.sec=59;
			s7sShowTime(timeout);
		}
		else
		{
			return 1;
		}
	}
	return 0;
}
void timeoutIncrement()
{
	if(timeout.sec==59)
	{
		if(timeout.min < 99)
		{
			timeout.min++;
			timeout.sec=0;
			s7sShowTime(timeout);
		}
	}
	else
	{
		timeout.sec++;
		s7sShowTime(timeout);
	}
}

inline static void idleLoop()
{
	uint8_t state = (PINB & 0x80)|(PIND &0x3);
	if(state != button.oldState)
	{
		if(!((state ^ button.oldState) & state)) // Кнопка нажата
		{
			switch (state ^ button.oldState)
			{
			case 1: //Button Min
				*((uint16_t*)&timeout) = eeprom_read_word(&EE_timerMin);
				break;
			case 2: //Button Max
				*((uint16_t*)&timeout) = eeprom_read_word(&EE_timerMax);
				break;
			case 0x80:
				button.oldState = state;
				loop.state = SET;
				return;
			}
			s7sShowTime(timeout);
		}
		else //Кнопка отжата
		{
			chrgStrat();
			loop.state=RUN;
		}
		button.oldState = state;
	}
}
inline static void runLoop()
{
	uint8_t state = (PINB & 0x80)|(PIND &0x3);
	if(state != button.oldState)
	{
		chrgStop();
		s7sClear();
		loop.state=IDLE;
		while(((PINB & 0x80)|(PIND &0x3))!=0x83);
		button.oldState = 0x83;
		return;
	}
	if(countdown == 0)
	{
		cli();
		countdown = calibr;
		sei();
		if(timeoutDecriment())
		{
			loop.state=IDLE;
			chrgStop();
		}
	}
}
inline static void setLoop()
{

	int8_t selected;
	timeout.min = 0;
	timeout.sec=0;
	while((PINB & 0x80)==0)// Пока не отпустили SET
	{
		uint8_t state = (PIND &0x3);
		selected = -1;
		if((state & 1)==0)
		{
			*((uint16_t*)&timeout) = eeprom_read_word(&EE_timerMin);
			selected = 0;
		}
		if((state & 2)==0)
		{
			if(selected == -1)
			{
				*((uint16_t*)&timeout) = eeprom_read_word(&EE_timerMax);
				selected = 1;
			}
			else
				selected = 3;
		}
		s7sShowTime(timeout);
	}
	countdown = 400;
	while(countdown);
	if(selected ==-1)
	{
		loop.state=IDLE;
		s7sClear();
		while(((PINB & 0x80)|(PIND &0x3))!=0x83);
		button.oldState = 0x83;
		return;
	}
	if (selected == 3)
	{
		//!TODO:insert timer calibration function here
		loop.state=IDLE;
		while(((PINB & 0x80)|(PIND &0x3))!=0x83);
		button.oldState = 0x83;
		return;
	}
	while((PINB & 0x80)!=0)// Пока не отпустили SET
	{
		uint8_t state = (PIND &0x3);
		if((state & 1) == 0)
		{
			timeoutDecriment();
			while((PIND & 1) ==0);
		}
		if((state & 2) == 0)
		{
			timeoutIncrement();
			while((PIND & 2) ==0);
		}
		cli();
		if(countdown >100)
			s7sClear();
		else
		{
			if(countdown==0)
			{
				countdown = 200;
			}
			else
			{
				s7sShowTime(timeout);
			}
		}
		sei();
	}
	if(selected==0)
	{
		//*((uint16_t*)&timeout) = eeprom_read_word(&EE_timerMin);
		eeprom_update_word(&EE_timerMin,*(uint16_t*)&timeout);
	}
	else
	{
		eeprom_update_word(&EE_timerMax,*(uint16_t*)&timeout);
	}
	while(((PINB & 0x80)|(PIND &0x3))!=0x83);
	button.oldState = 0x83;
	loop.state = IDLE;
}

void main(void)
{
	s7sInit();
	chrgPmpInit();

	my_delay(200);

  // Button init
	button.oldState = 0x83;
	button.timeout =0;
	DDRD &= ~(0x3);
	DDRB &= ~(0x80);
  // Logic init




	*((uint16_t*)&calibr) = eeprom_read_word(&EE_Calibr);



	SREG|=1<<7;
	s7s.dat[0]=16;
	s7s.dat[1]=16;
	s7s.dat[2]=16;
	s7s.dat[3]=16;
	s7s.point=3;


	for(;;)
	{
		switch (loop.state)
		{
		case RUN:
			runLoop();
			break;
		case IDLE:
			idleLoop();
			break;
		case SET:
			setLoop();
			break;
		}
	}

}

