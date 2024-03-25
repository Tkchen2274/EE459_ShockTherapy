// Functions for controlling LCD
#include "i2c.h"
#include <stddef.h>

void clear_screen(void){ // function to clear LCD screen
	unsigned char status = i2c_io(LCD_ADDR, (unsigned char[]){0x80, 0x01}, 2, NULL, 0);	//80 is control byte, 01 means clear
};

void lcd_stringout(const char *string) {
	unsigned char data[20];
	data[0] = 0x40;	//indicator for data
	int i = 1;
	while(*string && i<sizeof(data)){	//fill data array until you run out of data or you max the buffer
		data[i++] = *string++;
	}
	unsigned char status = i2c_io(LCD_ADDR, data, i, NULL, 0); //send it over i2c
};

void lcd_moveto(unsigned char pos) {
	unsigned char data[2] = {0x80, (pos | 0x40)}; //creates command for moving to specified pos
	unsigned char status = i2c_io(LCD_ADDR, data, 2, NULL, 0); //send it over i2c
};

void turn_on_cursor() {
	unsigned char data[2] = {0x80, 0x0F}; //turns on display, cursor, and blinking
	unsigned char status = i2c_io(LCD_ADDR, data, 2, NULL, 0); //send it over i2c
};
