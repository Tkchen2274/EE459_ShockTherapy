#include "i2c.h"
#include "keypad.h"

unsigned char getButton(){
	unsigned char wbuf[1];
	wbuf[0] = 0x03;	// Register for reading oldest pushed butotn
	unsigned char rbuf[1];
	unsigned char status = i2c_io(KEYPAD_ADDR, wbuf, 1, rbuf, 1);
	return rbuf[0];
}
