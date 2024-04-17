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
#include <stdio.h>
#include "i2c.h"
#include "lcd.h"
#include "audio.h"
#include "keypad.h"
#include "servo.h"
#include "rfid.h"
#include "uart.h"

unsigned char status;
void handle_button_press(char);
volatile unsigned char pir_detected = 0;
volatile unsigned char door_closed = 0;


int main(void)
{
	unsigned char count = 0; // used for counting # of fifo reads
	unsigned int loop_count = 0;	//used for counting loop iterations
	unsigned char button_handled = 0;	// flag for keeping track of button presses
	unsigned char touch_handled = 0; 	// flag for keeping track of detected touches
	unsigned int lock_timeout = 0;
	unsigned char pass[4] = {1, 2, 3, 4};	// array for holding password CHANGE ME TO EEPROM
    unsigned char lock_threshold = 3;	// CHANGE ME TO EEPROM
    unsigned char attempt[4];	// array for storing password attempt

	

	unsigned char pass_main_flag = 0;	// flags for setting auth modes
	
	analog_timer_init();	// Initializes analog polling
	servo_init();
	lcd_init();
	adc_init();	// init adc for keypad
    uart_init();	// init serial connection with rpi
	sei();			//enables interrupts
	
	DDRB &= ~(1 << DDB7);	//Blue button as input
    DDRB &= ~(1 << DDB2);	//Red button as input
    DDRD &= ~(1 << DDD4);	//PIR sensor as input
	DDRB &= ~(1 << DDB0);	//Reed switch as input


    //Pull up resistor for buttons
    PORTB |= (1 << PB7);
    PORTB |= (1 << PB2);
	PORTB |= (1 << PB0);

	
	DDRC |= 1 << 0;		// Set PC0 as output (red LED)
	DDRB |= 1 << 0;		// Set PB1 as output
	DDRD |= 1 << 5;		// Shocker output
	TCCR2B |= (0b001 << CS20);	// No prescalar

	// PIR pin change initialization:
	// Enable pin change interrupt on PD4
    PCMSK2 |= (1 << PCINT20);
    // Enable pin change interrupt 2
    PCICR |= (1 << PCIE2);

	PCMSK0 |= (1 << PCINT0);
	PCICR |= (1 << PCIE0);

	i2c_init(BDIV);
	_delay_ms(100);	// Needs a delay so that the audio module can boot up
	clear_screen();
	play_track(1);
	_delay_ms(1);
	lcd_stringout("Enter password:");
	turn_on_cursor();

    lcd_moveto(20);
	lcd_stringout("  locked.");


	
	unsigned char button;
	unsigned char col1;
	unsigned char col2;
	unsigned char col3;
	_delay_ms(1);

	while (1){

		/*
		face_auth_flag = //receive from rpi
		touch_auth_flag = //receive from rpi
		rfid_auth_flag = //receive from rpi
		pin_auth_flag = //receive from rpi

		enable_auth_flag = //receive from rpi

		total_auth_flag = face_auth_flag + touch_auth_flag + rfid_auth_flag + pin_auth_flag;
		*/


		if(touched && !touch_handled){
			PORTC |= 1 << 0;	// indicate on red LED
			PORTD |= 1 << 5;	// administer shock. Disable after demo.
			play_pause();		//change to siren sound and play for about 20 seconds
			touch_handled=1;	// this is to ensure the sound is only played once per touch. maybe change this so sound keeps playing
			//OCR1A = 1600;	// unlocked
		}
		else if(!touched){ 
			PORTC &= ~(1 << 0);	// Turn off the LED when no longer touching
			PORTD &= ~(1 << 5);	// relieve shock
			touch_handled = 0;
			//OCR1A = 1000;	// locked
		}

		if((PINB & (1<<7))==0){	//doorbell button pressed
				play_track(1);
				handle_button_press(7);
				//uart_transmit(0x01);	// request face recog. this would be different because needs to show image to user on website
		}


		//Only if face_auth_flag = 1
		if((PINB & (1<<2))==0){	//image capture button pressed
				uart_transmit(0x01);	// request face recog
				uart_transmit(0x02);	// request finger verf. Remove later
				handle_button_press(2);
				lock_timeout = 0;
		}

		//Only if pin_auth_flag = 1
		col1 = adc_sample(1);	// sample each keypad column
		col2 = adc_sample(2);
		col3 = adc_sample(3);
		
		if(col1 | col2 | col3){ // If a button is pressed, the adc result will be non-zero
			if(!button_handled){	// If we haven't handled it already
				lcd_moveto(15+count);	// prepare cursor on 3rd row	
				if(col1){
					switch(col1){
						case 83:
						case 84:
								attempt[count] = 1;
								lcd_stringout("1");
								count++;
								break;
						case 128:
								attempt[count] = 4;
								lcd_stringout("4");
								count++;
								break;
						case 169:
						case 170:
								attempt[count] = 7;
								lcd_stringout("7");
								count++;
							break;
					}
				}
				if(col2){
					switch(col2){
						case 83:
						case 84:
								attempt[count] = 2;
								lcd_stringout("2");
								count++;
								break;
						case 128:
								attempt[count] = 5;
								lcd_stringout("5");
								count++;
								break;
						case 169:
								attempt[count] = 8;	
								lcd_stringout("8");
								count++;
								break;
						case 230:
						case 231:
								attempt[count] = 0;
								lcd_stringout("0");
								count++;
								break;
					}
				}
				if(col3){
					switch(col3){
						case 83:
						case 84:
								attempt[count] = 3;
								lcd_stringout("3");
								count++;
								break;
						case 128:
								attempt[count] = 6;
								lcd_stringout("6");
								count++;
								break;
						case 169:
						case 170:
								attempt[count] = 9;
								lcd_stringout("9");
								count++;
								break;
					}
				}
				button_handled = 1;
				lock_timeout = 0;
				if (count == 4) {  // check attempt
						unsigned char face_flag = 1;  // Assume true initially
						for (unsigned char i = 0; i < 4; i++) {
								if (pass[i] != attempt[i]) {
										face_flag = 0;  // Set to false on any mismatch
										break;  // Exit the loop as no need to check further
								}
						}

						lcd_moveto(30);
						if (face_flag) {
								pass_main_flag = 1;
								lcd_stringout("right");
						} 
						else {
								lcd_stringout("wrong");
						}
						count = 0;
						lcd_moveto(15);	// clear pass
						lcd_stringout("    ");
				}
			}
		}
		else if (button_handled){	// If the buttons have been released, reset the flag
			button_handled = 0;
		}
		if(name_done){	// face regonition result received
				if(facebuf[0] == '\0'){
						lcd_moveto(84);
						lcd_stringout("face: wrong");
				}
				else{
						lcd_moveto(84);
						lcd_stringout("               ");
						lcd_moveto(84);	// row 4
						lcd_stringout("face: ");
						lcd_stringout(facebuf);
						name_done = 0;
				}
		}
		if(finger_done){
				if(finger_done == 1){	// valid finger
						lcd_moveto(99);
						lcd_stringout("fin:v");
						//increment counter
				}
				else if(finger_done == 2){	// invalid finger
						lcd_moveto(99);
						lcd_stringout("fin:i");
				}
				finger_done = 0;
		}
		if(rfid_done){	// rfid flag
				if(rfid_done == 1){	// valid rfid
						lcd_moveto(81);
						lcd_stringout("r:v");
						//increment counter
				}
				else if(rfid_done == 2){	// invalid rfid
						lcd_moveto(81);
						lcd_stringout("r:i");
				}
				rfid_done = 0;
		}
		if(pir_detected && (lock_timeout > 200)){	// 5ms*200 = 1s timeout
				play_track(4);	// hello there
				//lock_timeout = 0;
				pir_detected = 0;

				//if touch_auth_flag = 1
				//send packet to run python file for touch id
				//uart_transmit(0x02);	// request finger verf
		}


		if((lock_timeout > 200) && door_closed){	// 10s timeout
				_delay_ms(5000);
				OCR1A = 1000;	// locked
				lcd_moveto(20);
				lcd_stringout("  locked.");
				lock_timeout = 0;
				face_main_flag = 0;
				pass_main_flag = 0;
				touch_main_flag = 0;
				rfid_main_flag = 0;
				lcd_moveto(99);
				lcd_stringout("fin:i");
				lcd_moveto(84);
				lcd_stringout("face: wrong");
				lcd_moveto(30);
				lcd_stringout("wrong");
				play_track(5);	// access denied
		}


		else if((face_main_flag+pass_main_flag+touch_main_flag+rfid_main_flag)>=lock_threshold){
				lcd_moveto(20);
				lcd_stringout("unlocked.");
				OCR1A = 1600;	// unlocked
				lock_timeout = 0;
				face_main_flag = 0;
				pass_main_flag = 0;
				touch_main_flag = 0;
				rfid_main_flag = 0;
				play_track(6);	// access granted
		}
		
		lcd_moveto(64);	// row 2
		char buf[16];
		sprintf(buf, "%3d %3d %3d t:%1d", col1, col2, col3, touched);
		lcd_stringout(buf);
		_delay_ms(5);
		loop_count++;
		lock_timeout++;
	}
	return 0;   /* never reached */
}

void handle_button_press(char bit){	// Function to check for button press
	_delay_ms(5);
	while((PINB & (1 << bit)) == 0){	
		_delay_ms(5);
	}
}

// ISR for Reed switch:
ISR(PCINT0_vect) {
    uint8_t currentState = PINB & (1 << PB0);
    if (currentState) {	// just became high
		door_closed = 1;
	}
}

// ISR for passive infrared sensor:
ISR(PCINT2_vect) {
    uint8_t currentState = PIND & (1 << PD4);
    if (currentState) {	// just became high
		pir_detected = 1;
	}
}
