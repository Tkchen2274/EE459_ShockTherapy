/*************************************************************
* EE459 Project (shock therapy) 
* Riad Alasadi, Arda Caliskan, Tyler Chen
*
*
*
*************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include "touch_detection.h"

int main(void)
{
	touch_pwm_init();
	DDRC |= 1 << 0;		// Set PC0 as output (red LED)
	DDRB |= 1 << 1;		// Set PB1 as output
	TCCR2B |= (0b001 << CS20);	// No prescalar
	while (1) {
		if(touched != 0){	// Flag for touch detection
			PORTB |= 1 << 0;	// for testing purposes, lights up LED
			_delay_ms(1000);
			touched = 0;
		}
		else{
			PORTC &= ~(1 << PC0);	// Turn off the LED when no longer touching
		}
		PORTC |= 1 << PC0;      // Set PC0 to a 1
		PORTB |= 1 << 1;
		_delay_us(40);
		PORTC &= ~(1 << PC0);   // Set PC0 to a 0
		PORTB &= ~(1 << 1);
		_delay_us(1);	
	}
	return 0;   /* never reached */
}
