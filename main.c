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
#include "keypad.h"
#include "servo.h"
#include "rfid.h"
#include "uart.h"

unsigned char status;

int main(void)
{
	unsigned char count = 0; // used for counting # of fifo reads
	unsigned int loop_count = 0;	//used for counting loop iterations
	unsigned char button_handled = 0;	// flag for keeping track of button presses
	unsigned char touch_handled = 0; 	// flag for keeping track of detected touches
						//
	analog_timer_init();	// Initializes analog polling
	servo_init();
	lcd_init();
	adc_init();	// init adc for keypad
    uart_init();	// init serial connection with rpi
	sei();			//enables interrupts
	
	DDRC |= 1 << 0;		// Set PC0 as output (red LED)
	DDRB |= 1 << 0;		// Set PB1 as output
	DDRD |= 1 << 5;		// Shocker output
	TCCR2B |= (0b001 << CS20);	// No prescalar
	i2c_init(BDIV);
	_delay_ms(100);	// Needs a delay so that the audio module can boot up
	play_track(5);
	clear_screen();
	_delay_ms(1);
	lcd_stringout("Enter password:");
	turn_on_cursor();

	
	unsigned char button;
	unsigned char col1;
	unsigned char col2;
	unsigned char col3;
	_delay_ms(1);

	while (1){
		if(touched && !touch_handled){
			PORTC |= 1 << 0;	// indicate on red LED
			PORTD |= 1 << 5;	// administer shock
			play_pause();
			touch_handled=1;	// this is to ensure the sound is only played once per touch
			OCR1A = 1600;	// unlocked
		}
		else if(!touched){ 
			PORTC &= ~(1 << 0);	// Turn off the LED when no longer touching
			PORTD &= ~(1 << 5);	// relieve shock
			touch_handled = 0;
			OCR1A = 1000;	// locked
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
							uart_transmit(0x01);	// FIX ME LATER, packet that requests a face detection result
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
							uart_transmit(0x02);	// FIX ME LATER, packet that requests a finger result
							break;
						case 128:
							lcd_stringout("5");
							count++;
							break;
						case 169:
							lcd_stringout("8");
							count++;
							break;
						case 230:
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
		if(name_done){	// face regonition result received
				if(facebuf[0] == '\0'){
						lcd_moveto(84);
						lcd_stringout("invalid face");
				}
				else{
						lcd_moveto(84);
						lcd_stringout("                 ");
						lcd_moveto(84);	// row 4
						lcd_stringout(facebuf);
						name_done = 0;
				}
		}
		if(finger_done){
				if(finger_done == 1){	// valid finger
						lcd_moveto(94);
						lcd_stringout("f: valid");
				}
				else if(finger_done == 2){	// invalid finger
						lcd_moveto(94);
						lcd_stringout("f: invalid");
				}
				finger_done = 0;
		}
		
		
		lcd_moveto(64);	// row 2
		char buf[16];
		sprintf(buf, "%3d %3d %3d t:%1d", col1, col2, col3, touched);
		lcd_stringout(buf);
		_delay_ms(5);
		loop_count++;
	}
	return 0;   /* never reached */
}
