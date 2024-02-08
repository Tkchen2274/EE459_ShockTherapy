/*************************************************************
* EE459 Project (shock therapy) 
* Riad Alasadi, Arda Caliskan, Tyler Chen
*
*
*
*************************************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "touch_detection.h"

void touch_pwm_init(void);
void analog_timer_init(void);

int main(void)
{
	touch_pwm_init();	// Initializes pwm output (deprecated)
	analog_timer_init();	// Initializes analog polling
	sei();			//enables interrupts
	DDRC |= 1 << 0;		// Set PC0 as output (red LED)
	DDRB |= 1 << 0;		// Set PB1 as output
	TCCR2B |= (0b001 << CS20);	// No prescalar
	while (1){
		if(touched){
			PORTB |= 1 << 0;	//makes the dips
		}
		else{
			PORTB &= ~(1 << 0);	// Turn off the LED when no longer touching
		}	
	}
	return 0;   /* never reached */
}
