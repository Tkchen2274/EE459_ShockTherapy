#include <avr/io.h>
#include "touch_detection.h"

volatile char touched = 0;

void touch_pwm_init(void){  // output is on PB3
	DDRB |= (1<<3);
	TCCR2A |= (0b11 << WGM20); // fast pwm mode
	TCCR2A |= (0b10 << COM2A0); // clear OC2A on compare match
	// 001 into CS20 of TCCR2B for no prescale
	OCR2A = 250;
}
