#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

#define F_CPU 7372800
#define BAUD 38400
#define BRC ((F_CPU/16/BAUD) - 1)

volatile unsigned char byte_count = 0;
volatile unsigned char face_flag = 0;
volatile unsigned char facebuf[21];
volatile unsigned char name_done = 0;
volatile unsigned char finger_flag = 0;
volatile unsigned char finger_done = 0;

void uart_init() {
    // Set the baud rate
    UBRR0H = (BRC >> 8);
    UBRR0L = BRC;

    // Enable UART receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	// Enable the RX complete interrupt
    UCSR0B |= (1 << RXCIE0);
}

void uart_transmit(unsigned char data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));

    // Put data into buffer, sends the data
    UDR0 = data;
}

/*
unsigned char uart_receive() {
    // Wait for data to be received
    while (!(UCSR0A & (1 << RXC0)));

    // Get and return received data from buffer
    return UDR0;
}
*/

ISR(USART_RX_vect) {
		unsigned char received_data = UDR0;  // Read the received data into a global variable
		
		if((byte_count == 0)&&(received_data == 0x01)){	// Receiving face result
				face_flag = 1;
				byte_count++;
		}
		else if((byte_count == 0)&&(received_data == 0x02)){	// Receiving fingerprint result
				finger_flag = 1;
				byte_count++;
		}
		else if(face_flag){	// receiving name
				if(received_data == 0){	// invalid face
						facebuf[0] = '\0';
						name_done = 1;
						byte_count = 0;
						face_flag = 0;
				}
				else if(received_data == 0xFF){	// terminate byte received (FF)
						facebuf[byte_count-1] = '\0';
						name_done = 1;
						face_flag = 0;
						byte_count = 0;
				}
				else{	// valid data
						facebuf[byte_count-1] = received_data;
						byte_count++;
				}
		}
		else if(finger_flag){
				if(received_data == 0){	// invalid finger
						finger_done = 2;	// result done, and is invalid.
				}
				else if(received_data == 1){	// valid finger
						finger_done = 1;	// result done, and is valid.
				}
				finger_flag = 0;
				byte_count = 0;	// only one packet is received, so immediately terminate
		}
		
}
