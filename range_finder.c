#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "range_finder.h"

void rangefinder_init(){
	TCCR2B |= (1 << WGM21);	// Set to CTC mode
	TIMSK2 |= (1 << OCIE2A);
	OCR2A = 189	// prescalar 1024 -> triggers if range >200 cm (us/58)=cm
}

ISR{
