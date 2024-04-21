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
void EEPROM_write(unsigned int, unsigned char);
unsigned char EEPROM_read(unsigned int);
volatile unsigned char pir_detected = 0;
volatile unsigned char door_closed = 0;


int main(void)
{
	unsigned char count = 0; // used for counting # of fifo reads
	unsigned int loop_count = 0;	//used for counting loop iterations
	unsigned char button_handled = 0;	// flag for keeping track of button presses
	unsigned char touch_handled = 0; 	// flag for keeping track of detected touches
	unsigned int lock_timeout = 0;
    EEPROM_write(1002, 1234 & 0xFF);
	EEPROM_write(1001, 1234 >> 8);
    unsigned char auth_byte = EEPROM_read(1000);	// EEPROM read of authentication byte
    unsigned int password = EEPROM_read(1002);	// Least signficant 8 bits of password
    password |= EEPROM_read(1001) << 8;	// Most signifcant 8 bits of password
    
	unsigned char pass[4] = {password/1000, (password/100)%10, (password/10)%10, password%10};	// array for holding password
    
    EEPROM_write(1003, 3);	// store threshold in EEPROM
	unsigned char lock_threshold = EEPROM_read(1003);	// CHANGE ME TO EEPROM
    unsigned char attempt[4];	// array for storing password attempt

	unsigned char correct_auth = 0;	// variable for storing attempted authentications

	unsigned char keypad_tries = 0;
	unsigned char face_tries = 0;
	unsigned char touch_tries = 0;
	unsigned char rfid_tries = 0;
	//Keybad is MSb
	//then face id
	//then finger
	//rfid is LSb
	

	unsigned char pass_main_flag = 0;	// flags for setting auth modes
	
	analog_timer_init();	// Initializes analog polling
	servo_init();
	lcd_init();
	adc_init();		// init adc for keypad
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
			uart_transmit(0x04);	// take picture of intruder
			PORTC |= 1 << 0;	// indicate on red LED
			PORTD |= 1 << 5;	// administer shock. Disable after demo.
		    play_track(2);		// change to siren sound and play for about 20 seconds
			touch_handled = 1;	// this is to ensure the sound is only played once per touch. maybe change this so sound keeps playing	
			//OCR1A = 1600;	// unlocked
		}
		else if(!touched){ 
			PORTC &= ~(1 << 0);	// Turn off the LED when no longer touching
			PORTD &= ~(1 << 5);	// relieve shock
			touch_handled = 0;
			//OCR1A = 1000;	// locked
		}

		if((PINB & (1<<7))==0){	//doorbell button pressed
				uart_transmit(0x03); //take picture of guest
				play_track(5);	// ding dong
				handle_button_press(7);
				OCR1A = 1600;	// unlocked
		}

		//Only if face_auth_flag = 1
		if((PINB & (1<<2))==0){	//image capture button pressed
				uart_transmit(0x01);	// request face recog
				uart_transmit(0x02);	// request finger verf. Remove later
				handle_button_press(2);
				lock_timeout = 0;
				OCR1A = 1000;	// unlocked
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
						unsigned char pass_flag = 1;  // Assume true initially
						for (unsigned char i = 0; i < 4; i++) {
								if (pass[i] != attempt[i]) {
										pass_flag = 0;  // Set to false on any mismatch
										break;  // Exit the loop as no need to check further
								}
						}

						lcd_moveto(30);
						if (pass_flag) {
								pass_main_flag = 1;
								lcd_stringout("right");
								correct_auth |= (1<<3);
						} 
						else {
								lcd_stringout("wrong");
								keypad_tries++;
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
						face_tries++;

				}
				else{
						lcd_moveto(84);
						lcd_stringout("               ");
						lcd_moveto(84);	// row 4
						lcd_stringout("face: ");
						lcd_stringout(facebuf);
						name_done = 0;
						correct_auth |= (1<<2);
				}
		}
		if(finger_done){
				if(finger_done == 1){	// valid finger
						lcd_moveto(99);
						lcd_stringout("fin:v");
						correct_auth |= (1<<1);
						
				}
				else if(finger_done == 2){	// invalid finger
						lcd_moveto(99);
						lcd_stringout("fin:i");
						touch_tries++;
				}
				finger_done = 0;
		}
		if(rfid_done){	// rfid flag
				if(rfid_done == 1){	// valid rfid
						lcd_moveto(81);
						lcd_stringout("r:v");
						correct_auth |= (1<<0);
						
				}
				else if(rfid_done == 2){	// invalid rfid
						lcd_moveto(81);
						lcd_stringout("r:i");
						rfid_tries++;
						
				}
				rfid_done = 0;
		}
		if(passwd_done){	// need to update password
				pass[0] = passwd_buf / 1000;
				pass[1] = (passwd_buf / 100) % 10;
				pass[2] = (passwd_buf / 10) % 10;
				pass[3] = passwd_buf % 10;
				EEPROM_write(1002, passwd_buf & 0xFF);	// store in eeprom
				EEPROM_write(1001, passwd_buf >> 8);
				passwd_done = 0;
				lcd_moveto(70);
				lcd_stringout("pass updated");
				_delay_ms(3000);
				lcd_moveto(70);
				lcd_stringout("            ");
		}
		if(auth_done){
				EEPROM_write(1000, enable_auth_flag);
				auth_done = 0;
				lcd_moveto(70);
				lcd_stringout("settings upd.");
				_delay_ms(3000);
				lcd_moveto(70);
				lcd_stringout("             ");
		}
		if(lock_done){	// this means to unlock
				lcd_moveto(20);
				lcd_stringout("unlocked.");
				lock_timeout = 0;
				correct_auth = 0;
				play_track(4);	// access granted
				OCR1A = 1600;	// unlocked
				lock_done = 0;
		}
		if(addedtouch_done){
				lcd_moveto(70);
				lcd_stringout("finger added");
				_delay_ms(3000);
				lcd_moveto(70);
				lcd_stringout("            ");
				addedtouch_done = 0;
		}
		if(addedrfid_done){
				lcd_moveto(70);
				lcd_stringout("rfid added");
				_delay_ms(3000);
				lcd_moveto(70);
				lcd_stringout("          ");
				addedrfid_done = 0;
		}
		if(pir_detected && (lock_timeout > 200)){	// 50ms*200 = 10s timeout
				//play_track(1);	// hello there
				//lock_timeout = 0;
				pir_detected = 0;

				//if touch_auth_flag = 1
				//send packet to run python file for touch id
				//uart_transmit(0x02);	// request finger verf
		}


		if ((keypad_tries > 5) || (face_tries > 5) || (touch_tries > 5) || (rfid_tries > 5)){
			uart_transmit(0x04); //take picture of intruder
			for(int i; i < 5; i++){
				play_track(3);
				_delay_ms(2000);
			}
			keypad_tries = 0;
			face_tries = 0;
			touch_tries = 0;
			rfid_tries = 0;

		}

		if((lock_timeout > 1000)){	// 5s timeout
				_delay_ms(100);
				OCR1A = 1000;	// locked
				lcd_moveto(20);
				lcd_stringout("  locked.");
				lock_timeout = 0;
				correct_auth = 0;
				lcd_moveto(99);
				lcd_stringout("fin:i");
				lcd_moveto(84);
				lcd_stringout("face: wrong");
				lcd_moveto(30);
				lcd_stringout("wrong");
				play_track(3);	// access denied
				OCR1A = 1000;	// locked
		}

		else if((((correct_auth & 0x8)>>3)+((correct_auth & 0x4)>>2)+((correct_auth & 0x2)>>1)+(correct_auth & 0x1))>=lock_threshold){
		//else if((enable_auth_flag & correct_auth) == enable_auth_flag){
				lcd_moveto(20);
				lcd_stringout("unlocked.");
				lock_timeout = 0;
				play_track(4);	// access granted
				OCR1A = 1600;	// unlocked
				correct_auth = 0;
		}
		
		lcd_moveto(64);	// row 2
		char buf[16];
		sprintf(buf, "%1d t:%1d", door_closed, touched);
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

// Function to write a byte to the EEPROM
void EEPROM_write(unsigned int uiAddress, unsigned char ucData) {
    /* Wait for completion of previous write */
    while (EECR & (1 << EEPE));

    /* Set Programming mode */
    EECR = (0 << EEPM1) | (0 << EEPM0);

    /* Set up address and data registers */
    EEAR = uiAddress;
    EEDR = ucData;

    /* Write logical one to EEMPE */
    EECR |= (1 << EEMPE);

    /* Start eeprom write by setting EEPE */
    EECR |= (1 << EEPE);
}

// Function to read a byte from the EEPROM
unsigned char EEPROM_read(unsigned int uiAddress) {
    /* Wait for completion of previous write if any */
    while (EECR & (1 << EEPE));

    /* Set up address register */
    EEAR = uiAddress;

    /* Start EEPROM read by writing EERE */
    EECR |= (1 << EERE);

    /* Return data from Data Register */
    return EEDR;
}

// ISR for Reed switch:
ISR(PCINT0_vect) {
    uint8_t currentState = PINB & (1 << PB0);
    if (currentState) {	// just became high
		door_closed = 1;
	}
	else{	// just opened up
		door_closed = 0;
	}
}

// ISR for passive infrared sensor:
ISR(PCINT2_vect) {
    uint8_t currentState = PIND & (1 << PD4);
    if (currentState) {	// just became high
		pir_detected = 1;
	}
}
