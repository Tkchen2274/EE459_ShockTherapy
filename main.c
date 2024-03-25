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
#include "rfid.h"
#include "keypad.h"

unsigned char status;

int main(void)
{
	touch_pwm_init();	// Initializes pwm output (deprecated)
	analog_timer_init();	// Initializes analog polling
	spi_init();
	mfrc522_soft_reset();
	sei();			//enables interrupts
	
	DDRC |= 1 << 0;		// Set PC0 as output (red LED)
	DDRB |= 1 << 0;		// Set PB1 as output
	DDRC |= 1 << 2;		// Shocker output
	TCCR2B |= (0b001 << CS20);	// No prescalar
	i2c_init(BDIV);
	_delay_ms(1000);	// Needs a delay so that the audio module can boot up
	play_track(4);
	_delay_ms(1000);	// Delay to hear the sound that has been skipped to
	lcd_moveto(2);
	clear_screen();
	_delay_ms(1);
	turn_on_cursor();
	//write_char('b');

	// Configure RFID settings
    	//mfrc522_configure_antenna_gain(3);  // Set antenna gain to maximum (example: gain = 3)
    	//mfrc522_configure_agc(1);           // Enable AGC
    	//mfrc522_configure_tx_ask(1);        // Enable TXASK
	//mfrc522_configure_sensitivity(2);   // Set sensitivity level (example: level = 2)
	
	unsigned char button;

	while (1){
		mfrc522_test();
		if(touched){
			PORTB |= 1 << 0;	//touched
		}
		else{
			PORTB &= ~(1 << 0);	// Turn off the LED when no longer touching
		}
		button = getButton();
		
		//play_pause();
		PORTC ^= 1 << 2;	// toggle shock	
		//lcd_moveto(1);
		clear_screen();
		_delay_ms(500);
		lcd_stringout("bruh");
		play_track(5);
		_delay_ms(500);
	}
	return 0;   /* never reached */
}
