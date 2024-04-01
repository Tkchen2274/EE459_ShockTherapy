#include <avr/io.h>
#include <util/delay.h>

void servo_init(void) {
    // Clear on compare match, set at BOTTOM (non-inverting mode)
    // Fast PWM mode with ICR1 as top
    TCCR1A |= (1 << WGM11) | (1 << COM1A1);
    TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11); // Prescaler = 8

    // Set ICR1 so that PWM frequency = 50Hz (Period = 20ms)
    ICR1 = 39999;

    // Set PB1 (OC1A, Digital Pin 1) as output
    DDRB |= (1 << PB1);
}

/*

void setServoAngle(unsigned int angle) {
    // Calculate OCR1A value for a given angle
    unsigned int ocrValue = ((angle / 180.0) * (4999 - 999)) + 999;
    OCR1A = ocrValue;
    OCR1A should be 1000 for 0 and 3000 for 90
}


int main(void) {
	  initPWM();

    while(1) {
        // Sweep the servo from 0 to 180 degrees
        for(unsigned int angle = 0; angle <= 180; angle++) {
            setServoAngle(angle);
            _delay_ms(15); // Delay to allow the servo to reach the position
        }
        // And back from 180 to 0 degrees
        for(unsigned int angle = 180; angle > 0; angle--) {
            setServoAngle(angle);
            _delay_ms(15);
        }
    }
}
*/
