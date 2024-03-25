#define KEYPAD_ADDR 0x96	// Actual 7-bit address is 4B, this is shifted up to become 8

//Map to the various registers on the Keypad
enum keypadRegisters {
  KEYPAD_ID = 0x00,
  KEYPAD_VERSION1, // 0x01
  KEYPAD_VERSION2, // 0x02
  KEYPAD_BUTTON, // 0x03
  KEYPAD_TIME_MSB, // 0x04
  KEYPAD_TIME_LSB, // 0x05
  KEYPAD_UPDATE_FIFO, // 0x06
  KEYPAD_CHANGE_ADDRESS, // 0x07
};

unsigned char getButton();
