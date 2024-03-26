#include "i2c.h"
#include "keypad.h"
#include <util/delay.h>
#include <stddef.h>

unsigned char getButton(){
	unsigned char status = i2c_io(KEYPAD_ADDR, (unsigned char[]){0x06, 0x01}, 2, NULL, 0);
	_delay_ms(1);
	unsigned char rbuf[1];
	status = i2c_io(KEYPAD_ADDR, (unsigned char[]){0x03}, 1, rbuf, 1);
	_delay_ms(1);
	return rbuf[0];
}
