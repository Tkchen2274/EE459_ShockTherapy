#include "uart.h"

#include <avr/interrupt.h>

#include <avr/io.h>

#define F_CPU 7372800
#define BAUD 38400
#define BRC ((F_CPU / 16 / BAUD) - 1)

volatile unsigned char byte_count = 0;
volatile unsigned char face_flag = 0;
volatile unsigned char facebuf[21];
volatile unsigned char name_done = 0;
volatile unsigned char finger_flag = 0;
volatile unsigned char finger_done = 0;
volatile unsigned char rfid_flag = 0;
volatile unsigned char rfid_done = 0;
volatile unsigned char passwd_flag = 0;
volatile unsigned char passwd_done = 0;
volatile unsigned int passwd_buf = 0; // buffer for storing 2 bytes of passwd
volatile unsigned char enable_auth_flag = 0;
volatile unsigned char lock_done = 0;
volatile unsigned char lock_flag = 0;
volatile unsigned char auth_done = 0;
volatile unsigned char auth_byte = 0; // variable for storing authentication byte
volatile unsigned char auth_flag = 0;
volatile unsigned char addedtouch_done = 0; // flag for "added password"
volatile unsigned char addedrfid_done = 0; // flag for "added rfid"
volatile unsigned char touch_main_flag = 0;
volatile unsigned char face_main_flag = 0;
volatile unsigned char rfid_main_flag = 0;

volatile unsigned int total_auth_flag = 0; // should be total of the authentication flags

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

ISR(USART_RX_vect) {
    unsigned char received_data = UDR0; // Read the received data into a global variable
    if ((byte_count == 0) && (received_data == 0x01)) { // Receiving face result
        face_flag = 1;
        byte_count++;
    } else if ((byte_count == 0) && (received_data == 0x02)) { // Receiving fingerprint result
        finger_flag = 1;
        byte_count++;
    } else if ((byte_count == 0) && (received_data == 0x03)) { // Receiving rfid result
        rfid_flag = 1;
        byte_count++;
    }
    // 0x04 passwd 2 bytes
    // 0x05 rpi sending selected auth 1 byte
    // 0x06 rpi sending lock/unlock 1 byte
    // 0x03 arduino to rpi request pic for doorbell
    // 0x04 arduino to rpi request pic for intruder
    else if ((byte_count == 0) && (received_data == 0x04)) { // receiving updated password
        passwd_flag = 1;
        byte_count++;
    } else if ((byte_count == 0) && (received_data == 0x05)) { // receiving message of selected auths
        auth_flag = 1;
        byte_count++;
    } else if ((byte_count == 0) && (received_data == 0x06)) { // receiving message to lock/unlock
        lock_flag = 1;
        byte_count++;
    } else if ((byte_count == 0) && (received_data == 0x07)) { // receiving message that touch has been added
        addedtouch_done = 1;
    } else if ((byte_count == 0) && (received_data == 0x08)) { // receiving message that touch has been added
        addedrfid_done = 1;
    } else if (face_flag) { // receiving name
        if (received_data == 0) { // invalid face
            facebuf[0] = '\0';
            name_done = 1;
            byte_count = 0;
            face_flag = 0;
        } else if (received_data == 0xFF) { // terminate byte received (FF)
            facebuf[byte_count - 1] = '\0';
            name_done = 1;
            face_flag = 0;
            byte_count = 0;
            face_main_flag = 1;

        } else { // valid data
            facebuf[byte_count - 1] = received_data;
            byte_count++;
        }
    } else if (finger_flag) {
        if (received_data == 0) { // invalid finger
            finger_done = 2; // result done, and is invalid.
        } else if (received_data == 1) { // valid finger
            finger_done = 1; // result done, and is valid.
            face_flag = 1;
            touch_main_flag = 1;
        }
        finger_flag = 0;
        byte_count = 0; // only one packet is received, so immediately terminate
    } else if (rfid_flag) {
        if (received_data == 0) { // invalid rfid
            rfid_done = 2; // result done, and is invalid.
        } else if (received_data == 1) { // valid rfid
            rfid_done = 1; // result done, and is valid.
            rfid_flag = 1;
            rfid_main_flag = 1;
        }
        rfid_flag = 0;
        byte_count = 0; // only one packet is received, so immediately terminate
    } else if (passwd_flag) {
        if (byte_count == 1) {
            passwd_buf = received_data << 8;
            byte_count++;
        } else if (byte_count == 2) { // last byte, so wrap up
            passwd_buf |= received_data;
            byte_count = 0;
            passwd_flag = 0;
            passwd_done = 1;
        }
    } else if (auth_flag) {
        enable_auth_flag = received_data;
        auth_done = 1;
        auth_flag = 0;
        byte_count = 0;
    } else if (lock_flag) {
        if (received_data) { // if received data is 1
            lock_done = 2; // notify main loop to unlock
        } else {
            lock_done = 1; // means deny
        }
        lock_flag = 0;
        byte_count = 0;
    }
}