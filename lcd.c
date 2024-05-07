// Functions for controlling LCD
#include "i2c.h"
#include <stddef.h>
#include <util/delay.h>

void lcd_init(void) {
    _delay_ms(50);
    unsigned char status = i2c_io(LCD_ADDR, (unsigned char[]) {0x80,0x38}, 2, NULL, 0);
    _delay_ms(1);
    status = i2c_io(LCD_ADDR, (unsigned char[]) {0x80,0x08}, 2, NULL, 0);
    _delay_ms(1);
    status = i2c_io(LCD_ADDR, (unsigned char[]) {0x80,0x01}, 2, NULL, 0);
    _delay_ms(15);
    status = i2c_io(LCD_ADDR, (unsigned char[]) {0x80,0x06}, 2, NULL, 0);
}

void clear_screen(void) { // function to clear LCD screen
    unsigned char status = i2c_io(LCD_ADDR, (unsigned char[]) {0x80,0x01},2,NULL,0); // 80 is control byte, 01 means clear
};

void lcd_stringout(const char * string) {
    unsigned char data[20]; // Can only print 20 characters at a time
    data[0] = 0x40; // indicator for data
    int i = 1;
    while ( * string && i < sizeof(data)) { // fill data array until you run out of
        // data or you max the buffer
        data[i++] = * string++;
    }
    unsigned char status = i2c_io(LCD_ADDR, data, i, NULL, 0); // send it over i2c
};

// 0 (0x00) to 19 (0x13) is row 1
// 64 (0x40) to  83 (0x53) is row 2
// 20 (0x14) to 39 (0x27) is row 3
// 84 (0x54) to 103 (0x67) is row 4
void lcd_moveto(unsigned char pos) {
    unsigned char data[2] = {0x80,(pos | 0x80)}; // creates command for moving to specified pos
    unsigned char status = i2c_io(LCD_ADDR, data, 2, NULL, 0); // send it over i2c
};

void turn_on_cursor() {
    unsigned char data[2] = {0x80,0x0F}; // turns on display, cursor, and
    // blinking
    unsigned char status = i2c_io(LCD_ADDR, data, 2, NULL, 0); // send it over i2c
};