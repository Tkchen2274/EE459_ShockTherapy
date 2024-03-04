// Functions for controlling LCD
#include "i2c.h"
#include <stddef.h>

void clear_screen(void){ // function to clear LCD screen
	unsigned char status = i2c_io(LCD_ADDR, (unsigned char[]){0xFE, 0x51}, 2, NULL, 0);	// FE is control prefix, 0x51 means clear screen
};

void write_char(unsigned char character) {
	unsigned char data[1] = {character}; //create array for storing singular character
	unsigned char status = i2c_io(LCD_ADDR, data, 1, NULL, 0); //send it over i2c
};
