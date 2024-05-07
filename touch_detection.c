#include "touch_detection.h"
#include <avr/interrupt.h>
#include <avr/io.h>

volatile char touched = 0;
volatile unsigned int analogcount = 0;
volatile unsigned char isrcount = 0;

void analog_timer_init(void) { // timer for polling analog flag
  // for polling
  TCCR0A |= (0b10 << WGM00); // CTC mode
  TCCR0B |= (0b100 << CS00); // prescalar 256
  TIMSK0 |= (1 << OCIE0A);   // enable interrupts
  OCR0A = 255;
  ACSR |= (1 << ACIE); // enabling analog interrupts
}

ISR(ANALOG_COMP_vect) { // Called when not touched
  analogcount++;        // im not being touched
}

ISR(TIMER0_COMPA_vect) { // Have I been touched?
  if ((isrcount % 12) == 0) {
    ACSR |= (1 << ACIE);
  } else if (((isrcount - 1) % 12) == 0) {
    ACSR &= ~(1 << ACIE);
    if (analogcount > 300) { // no, i have not been touched alot
      touched = 0;
    } else { // yes, i have been touched quite alot, therefore my count is low
      touched = 1;
    }
  }
  analogcount = 0;
  isrcount++;
}
