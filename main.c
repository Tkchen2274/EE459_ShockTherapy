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

#define FOSC 9830400            // Clock frequency = Oscillator freq.
#define BAUD 9600               // UART0 baud rate
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register
#define BDIV (FOSC / 100000 - 16) / 2 + 1    // Puts I2C rate just below 100kHz
#define LCD_ADDR 0x27

void touch_pwm_init(void);
void analog_timer_init(void);

unsigned char status;
unsigned char buf[2];

int main(void)
{
	touch_pwm_init();	// Initializes pwm output (deprecated)
	analog_timer_init();	// Initializes analog polling
	sei();			//enables interrupts
	DDRC |= 1 << 0;		// Set PC0 as output (red LED)
	DDRB |= 1 << 0;		// Set PB1 as output
	TCCR2B |= (0b001 << CS20);	// No prescalar
	i2c_init(BDIV);

	while (1){
		if(touched){
			PORTB |= 1 << 0;	//touched
		}
		else{
			PORTB &= ~(1 << 0);	// Turn off the LED when no longer touching
		}		
		buf[1] = 0x30;
		status = i2c_io(LCD_ADDR, buf, 1, NULL, 0);
	}
	return 0;   /* never reached */
}
