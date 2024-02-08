/*************************************************************
*       at328-0.c - Demonstrate simple I/O functions of ATmega328
*
*       Program loops turning PC0 on and off as fast as possible.
*
* The program should generate code in the loop consisting of
*   LOOP:   SBI  PORTC,0        (2 cycles)
*           CBI  PORTC,0        (2 cycles)
*           RJMP LOOP           (2 cycles)
*
* PC0 will be low for 4 / XTAL freq
* PC0 will be high for 2 / XTAL freq
* A 9.8304MHz clock gives a loop period of about 600 nanoseconds.
*
* Revision History
* Date     Author      Description
* 09/14/12 A. Weber    Initial Release
* 11/18/13 A. Weber    Renamed for ATmega328P
*************************************************************/

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRC |= 0x01;          // Set PORTC bit 0 for output
	DDRB |= 1 << 1;
	while (1) {
		PORTC |= 1 << PC0;      // Set PC0 to a 1
		PORTB |= 1 << 1;
		_delay_us(40);
		PORTC &= ~(1 << PC0);   // Set PC0 to a 0
		PORTB &= ~(1 << 1);
		_delay_us(1);	
	}
	return 0;   /* never reached */
}
