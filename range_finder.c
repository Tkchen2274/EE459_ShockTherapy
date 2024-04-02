#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "range_finder.h"

void rangefinder_init(){
	TCCR2B |= (1 << WGM21);	// Set to CTC mode
	TIMSK2 |= (1 << OCIE2A);
	OCR2A = 189	// prescalar 1024 -> triggers if range >200 cm (us/58)=cm
}

ISR(PCINT6_vect){	// triggered when echo signal changes
	unsigned char rout = PINB & (1 << PB6);
	if(rout){	// measurement has begun
		TCNT2 = 0;	// reset counter
		TCCR2B |= (0b111 << CS20);	// Prescalar 1024
		got_time = 0;	// time is not valid
	}
	else{	// measurement has concluded
		TCCR2B &= ~(0b111 << CS20);	// Stop counter
		got_time = 1;	// time is valid
		range_count = TCNT2;	// load count value
	}
}

ISR(TIMER2_COMPA_vect){	// triggered when max distance is exceeded
	TCCR2B &= ~(0b111 << CS20);	// stop counter
	got_time = 1;	// time is valid (out of range)
	range_count = 0;	// reset counter
}

