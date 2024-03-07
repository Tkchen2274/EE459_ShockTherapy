//-----------------------------------------------------------------------------
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include "audio.h"

void digitalWrite(uint8_t pin, uint8_t mode){ 
  
  if(mode == HIGH)
    PORTD |= (1 << pin);
  else if(mode == LOW) {
    PORTD &= ~(1 << pin);
  }

}

void pinMode(uint8_t pin, uint8_t mode) {
  if (mode == OUTPUT) {
    // Set pin as output (set the corresponding bit in DDRB to 1)
    DDRD |= (1 << pin);
  } else if (mode == INPUT) {
    // Set pin as input (clear the corresponding bit in DDRB to 0)
    DDRD &= ~(1 << pin);
  }
}

int digitalRead(uint8_t pin){
  if(PIND & (1 << pin)){
    return 1;
  }
  else {
    return 0;
  }
  
}

void audioInit()
{  
  pinMode(DOUT, OUTPUT);
  digitalWrite(DOUT, HIGH);
  pinMode(DCLK, OUTPUT);
  digitalWrite(DOUT, HIGH);

}

void sendCommand(unsigned int command)
{
  digitalWrite(DCLK, LOW);
  _delay_us(700);
  for(uint8_t i = 0; i < 16; i++)
  {
    _delay_us(100);
    digitalWrite(DCLK, LOW);
    digitalWrite(DOUT, LOW);
    if ((command & 0x8000) != 0)
    {
      digitalWrite(DOUT, HIGH);
    }
    _delay_us(100);
    digitalWrite(DCLK, HIGH);
    command = command<<1;
  }
}

/*
 * First sends the file that needs to be played
 * then it sends the play command
 * might need to handle when the trackNo 
 * does not exsit 
 */

void playTrack(unsigned int trackNo){
  sendCommand(trackNo);
  //sendCommand(OPCODE_PLAY_PAUSE);
}

// void Stop(){
//   sendCommand(OPCODE_STOP);
// }

/*
 * The volume range is 0xFFF0 to 0xFFF7
 * OPCODE_VOL_SET is set to 0xFFF then is 
 * concatenated with the number you input
 */
void VolumeSet(unsigned int vol){

  if (vol < 8){
    sendCommand(OPCODE_VOL_SET << 4 | vol);
    
  }

}

/* 
 * Main loop
 */
/*
int main(void) 
{
  audioInit();
  playTrack(1);
  while(1){
    int a = 1;
  
  }
  
}
*/

