#define FOSC 9830400            // Clock frequency = Oscillator freq.
#define BAUD 9600               // UART0 baud rate
#define MYUBRR FOSC/16/BAUD-1   // Value for UBRR0 register
#define BDIV (FOSC / 50000 - 16) / 2 + 1    // Puts I2C rate just below 100kHz
#define LCD_ADDR 0x78	// 3C is 7 bit address, 

#include <stdint.h>

extern void i2c_init(uint8_t);
extern uint8_t i2c_io(uint8_t, uint8_t *, uint16_t, uint8_t *, uint16_t);


