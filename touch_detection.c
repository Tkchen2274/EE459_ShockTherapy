#include <avr/io.h>
#include <avr/interrupt.h>
#include "touch_detection.h"

volatile char touched = 0;
volatile unsigned int analogcount = 0;

//void touch_pwm_init(void){ 	// output is on PB3
//	DDRB |= (1<<3);
//	TCCR2A |= (0b11 << WGM20);	// Fast PWM mode
//	TCCR2A |= (0b10 << COM2A0);	// clear OC2A on compare match
//	// 001 into CS20 of TCCR2B for no prescale
//	OCR2A = 250;
//}

void analog_timer_init(void){  // timer for polling analog flag
		// for polling
		TCCR0A |= (0b10 << WGM00);	// CTC mode
		TCCR0B |= (0b101 << CS00);	// prescalar 1024
		TIMSK0 |= (1 << OCIE0A);	// enable interrupts
		OCR0A = 255;
		//for generating interrupts
		ACSR |= (1 << ACIE);	//enabling analog interrupts
}

ISR(ANALOG_COMP_vect){	// Called when not touched
		analogcount++;	// im not being touched
}

ISR(TIMER0_COMPA_vect){	// Have I been touched?
		if(analogcount > 3000){	// no, i have not been touched alot
				touched = 0;
		}
		else{	// yes, i have been touched quite alot, therefore my count is low
				touched = 1;
		}
		analogcount = 0;
}
