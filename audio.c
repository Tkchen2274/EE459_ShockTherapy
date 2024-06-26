#define PLAY_PAUSE 2
#define NEXT 3
#include "audio.h"

#include <avr/io.h>
#include <stddef.h>
#include <util/delay.h>

unsigned char num_tracks = 5; // UPDATEME with the current number of tracks present on the SD card
unsigned char current_track = 5; // starts with last track

void play_pause(void) {
    DDRD |= (1 << PLAY_PAUSE); // Pull ADKEY1 down
    PORTD &= ~(1 << PLAY_PAUSE);
    _delay_ms(40); // Short press for play/pause
    DDRD &= ~(1 << PLAY_PAUSE); // Switch to input again for not pressed
}

void skip_track(void) {
    DDRD |= (1 << NEXT); // Pull ADKEY1 down
    PORTD &= ~(1 << NEXT);
    _delay_ms(60); // Short press for skipping track 10ms is like the minimum
    DDRD &= ~(1 << NEXT); // Switch to input again for not pressed
}

void play_track(unsigned char track) {
    if (track == current_track) {
        play_pause();
    } else {
        while (track != current_track) {
            skip_track();
            _delay_ms(60); // min delay for skip to register
            current_track--; // the internal pointer moves backwards from the newest
            // added track towards the oldest
            if (current_track < 1) {
                current_track = num_tracks;
            }
        }
    }
}