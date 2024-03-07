#include <stdint.h>

#define PC1             1                // trigger pin
#define PC0             0
#define MAX_DISTANCE    400              // in centimeters
#define MAX_TIMEOUT     0                // in mirco seconds
#define TEMP            19.307

int digitalRead(uint8_t);
void rangeFinderSet(void);
uint32_t micros(void);
uint32_t pulseIn(int, int, uint32_t);
float measureDist(void);
