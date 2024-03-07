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
#include <stddef.h>
#include "touch_detection.h"
#include <compat/twi.h>
#include "i2c.h"
#include "lcd.h"
#include "audio.h"

unsigned char status;

int main(void)
{
	touch_pwm_init();	// Initializes pwm output (deprecated)
	analog_timer_init();	// Initializes analog polling
	sei();			//enables interrupts
	DDRC |= 1 << 0;		// Set PC0 as output (red LED)
	DDRB |= 1 << 0;		// Set PB1 as output
	TCCR2B |= (0b001 << CS20);	// No prescalar
	i2c_init(BDIV);
	play_track(3);
	_delay_ms(1000);

	while (1){
		if(touched){
			PORTB |= 1 << 0;	//touched
		}
		else{
			PORTB &= ~(1 << 0);	// Turn off the LED when no longer touching
		}		
		//write_char(0x38);
		play_pause();
		//skip_track();
		_delay_ms(1000);
		//clear_screen();
	}
	return 0;   /* never reached */
}
