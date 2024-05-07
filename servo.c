#include <avr/io.h>

#include <util/delay.h>

void servo_init(void) {
    // Clear on compare match, set at BOTTOM (non-inverting mode)
    // Fast PWM mode with ICR1 as top
    TCCR1A |= (1 << WGM11) | (1 << COM1A1);
    TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11); // Prescaler = 8
    // Set ICR1 so that PWM frequency = 23Hz
    ICR1 = 39999;
    // Set PB1 (OC1A, Digital Pin 1) as output
    DDRB |= (1 << PB1);
}
