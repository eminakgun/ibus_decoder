#ifndef IBUS_DECODER_H
#define IBUS_DECODER_H

#include <stdint.h>
#include <stdbool.h>    

// module constants
#define IBUS_FRAME_LENGTH 32 // 32 bytes length frame
#define IBUS_NUM_OF_CHANNELS 14 // number of rc channels in the frame

uint16_t rc_channels[IBUS_NUM_OF_CHANNELS + 1];
bool ibus_failsafe;

void ibus_handle_byte(uint8_t byte, uint32_t time_ms);

#endif


