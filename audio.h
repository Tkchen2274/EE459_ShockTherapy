#include <stdint.h>

// Pin definitions

#define HIGH 1
#define LOW  0

#define OUTPUT 1
#define INPUT  0


#define PD2 2
#define PD3 3

#define DOUT  2                     // Data input from a microcontroller.
#define DCLK  3                     // Data clock from a microcontroller.

// Opcode definitions- the 2-wire serial (which is NOT I2C- it is close to SPI
//  without a CS or MISO, but the Arduino can't clock data slowly enough to
//  suit the widget when using the internal SPI peripheral).
//
#define OPCODE_PLAY_PAUSE  0xFFFE   // Play a track, or pause the current
                                    //  playback. Note that this requires that
                                    //  you first send a number from 0-511 to
                                    //  the module to indicate which track you
                                    //  wish to play back- simply sending the
                                    //  PLAY_PAUSE opcode has no effect other
                                    //  than pausing playback.
#define OPCODE_STOP        0xFFFF   // Stops the current playback. Subsequent
                                    //  toggling of the PLAY pin will start
                                    //  playback as though no previous track
                                    //  were playing.
#define OPCODE_VOL         0xFFF0   // Set the volume. 0xFFF0 is muted, 0xFFF7
                                    //  is max. Defaults to max on startup.

#define OPCODE_VOL_SET     0xFFF                                     

void digitalWrite(uint8_t , uint8_t);
void pinMode(uint8_t, uint8_t);
int digitalRead(uint8_t);
void audioInit(void);
void sendCommand(unsigned int);
void playTrack(unsigned int);
void Stop(void);
void VolumeSet(unsigned int);

