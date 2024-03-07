#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <time.h>
#include "range_finder.h"


int digitalRead(uint8_t pin){
  if(PINC & (1 << pin)){
    return 1;
  }
  else {
    return 0;
  } 
}


void rangeFinderSet(){

  DDRC |= (1 << PC1);
  DDRC &= ~(1 << PC0);

}

uint32_t micros() {
    return (uint32_t)((clock() * 1000000) / CLOCKS_PER_SEC);
}

// pulseIn function implementation in raw C
uint32_t pulseIn(int echoPin, int level, uint32_t maxDuration) {
    uint32_t startMicros = micros();
    uint32_t timeout = startMicros + maxDuration;

    // Wait for the pin to reach the target level, but with timeout to avoid infinite loop
    while (digitalRead(echoPin) != level) {
        if (micros() > timeout) {
            return 0; // Timeout reached, return 0 as failure indicator
        }
    }

    uint32_t startPulseMicros = micros(); // Record the time when the pulse starts

    // Wait for the pin to go back to the opposite level, indicating the end of the pulse
    while (digitalRead(echoPin) == level) {
        if (micros() > timeout) {
            return 0; // Timeout reached, return 0 as failure indicator
        }
    }

    uint32_t endPulseMicros = micros(); // Record the time when the pulse ends

    // Calculate the duration of the pulse
    return endPulseMicros - startPulseMicros;
}

float measureDist(){

  PORTC &= ~(1 << PC1);
  _delay_us(2);
  PORTC |= (1 << PC1);
  PORTC &= ~(1 << PC1);

  float speedOfSoundInCmPerMicroSec = 0.03313 + 0.0000606 * TEMP;

  unsigned long maxDistanceDurationMicroSec = 2.5 * MAX_DISTANCE / speedOfSoundInCmPerMicroSec;

  if (MAX_TIMEOUT > 0) {

    if(maxDistanceDurationMicroSec > MAX_TIMEOUT)
    maxDistanceDurationMicroSec = MAX_TIMEOUT; 
  }
  unsigned long durationMicroSec = pulseIn(PC0, 1, maxDistanceDurationMicroSec);
  
  float distanceCm = durationMicroSec / 2.0 * speedOfSoundInCmPerMicroSec;
  if (distanceCm == 0 || distanceCm > MAX_DISTANCE) {
    return -1.0 ;
  } else {
     return distanceCm;
  }
 

}


