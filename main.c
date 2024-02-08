/*************************************************************
* EE459 Project (shock therapy) 
* Riad Alasadi, Arda Caliskan, Tyler Chen
*
*************************************************************/

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRC |= 1 << 0;          // Set PORTC bit 0 for output
	DDRB |= 1 << 1;
	while (1) {
		if(flag != 0){	// Flag for touch detection
			PORTB |= 1 << 0;	// for testing purposes, lights up LED
			_delay_ms(1000);
			flag = 0;
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
