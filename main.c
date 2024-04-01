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
#include "servo.h"

unsigned char status;

int main(void)
{
	unsigned char count = 0; // used for counting # of fifo reads
	unsigned int loop_count = 0;	//used for counting loop iterations
	unsigned char button_handled = 0;	// flag for keeping track of button presses

	touch_pwm_init();	// Initializes pwm output (deprecated)
	analog_timer_init();	// Initializes analog polling
	spi_init();
	lcd_init();
	adc_init();	// init adc for keypad
	mfrc522_soft_reset();
	sei();			//enables interrupts
	
	DDRC |= 1 << 0;		// Set PC0 as output (red LED)
	DDRB |= 1 << 0;		// Set PB1 as output
	DDRD |= 1 << 5;		// Shocker output
	TCCR2B |= (0b001 << CS20);	// No prescalar
	i2c_init(BDIV);
	_delay_ms(1000);	// Needs a delay so that the audio module can boot up
	play_track(4);
	_delay_ms(1000);	// Delay to hear the sound that has been skipped to
	//lcd_moveto(2);
	clear_screen();
	_delay_ms(1);
	lcd_stringout("Enter password:");
	turn_on_cursor();
	//write_char('b');

	// Configure RFID settings
    	//mfrc522_configure_antenna_gain(3);  // Set antenna gain to maximum (example: gain = 3)
    	//mfrc522_configure_agc(1);           // Enable AGC
    	//mfrc522_configure_tx_ask(1);        // Enable TXASK
	//mfrc522_configure_sensitivity(2);   // Set sensitivity level (example: level = 2)
	
	unsigned char button;
	unsigned char col1;
	unsigned char col2;
	unsigned char col3;
	_delay_ms(1);

	while (1){
		//mfrc522_test();
		if((loop_count % 100)==0){	// every 2s
			play_pause();
		}
		if(touched){
			PORTC |= 1 << 0;	// indicate on red LED
			PORTD |= 1 << 5;	// administer shock
			OCR1A = 3000;
		}
		else{ 
			PORTC &= ~(1 << 0);	// Turn off the LED when no longer touching
			PORTD &= ~(1 << 5);	// relieve shock
			OCR1A = 0;
		}

		col1 = adc_sample(1);	// sample each keypad column
		col2 = adc_sample(2);
		col3 = adc_sample(3);
		
		if(col1 | col2 | col3){ // If a button is pressed, the adc result will be non-zero
			if(!button_handled){	// If we haven't handled it already
				lcd_moveto(20+count);	// prepare cursor on 3rd row	
				if(col1){
					switch(col1){
						case 83:
						case 84:
							lcd_stringout("1");
							count++;
							break;
						case 128:
							lcd_stringout("4");
							count++;
							break;
						case 169:
						case 170:
							lcd_stringout("7");
							count++;
							break;
					}
				}
				if(col2){
					switch(col2){
						case 83:
						case 84:
							lcd_stringout("2");
							count++;
							break;
						case 128:
							lcd_stringout("5");
							count++;
							break;
						case 169:
							lcd_stringout("8");
							count++;
							break;
						case 231:
							lcd_stringout("0");
							count++;
							break;
					}
				}
				if(col3){
					switch(col3){
						case 83:
						case 84:
							lcd_stringout("3");
							count++;
							break;
						case 128:
							lcd_stringout("6");
							count++;
							break;
						case 169:
						case 170:
							lcd_stringout("9");
							count++;
							break;
					}
				}
				button_handled = 1;
			}
		}
		else if (button_handled){	// If the buttons have been released, reset the flag
			button_handled = 0;
		}
		//button = getButton();
		//play_pause();
		//PORTD ^= 1 << 5;	// toggle shock	
		//lcd_moveto(1);
		//clear_screen();
		//_delay_ms(200);
		lcd_moveto(64);	// row 2
		char buf[16];
		sprintf(buf, "%3d %3d %3d t:%1d", col1, col2, col3, touched);
		lcd_stringout(buf);
		//play_track(5);	// bruh.mp3
		_delay_ms(5);
		loop_count++;
	}
	return 0;   /* never reached */
}
