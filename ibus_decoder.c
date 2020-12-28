#include <string.h>
#include <stdio.h>
#include "ibus_decoder.h"

#define IBUS_HEADER_LOW 0x20
#define IBUS_HEADER_HIGH 0x40

static bool decode(uint8_t ibus_frame[IBUS_FRAME_LENGTH], uint16_t channels[IBUS_NUM_OF_CHANNELS + 1])
{
    uint32_t checksum = 96; // 0x20 + 0x40

    if ((ibus_frame[0] != IBUS_HEADER_LOW) || (ibus_frame[1] != IBUS_HEADER_HIGH)) {
        return false;
    }
    // ibus_frame indexes = 0 ..... 30, 31
    for (uint8_t ch = 1; ch <= IBUS_NUM_OF_CHANNELS; ch += 1){
        // we don't use 0th and 1th index in ibus_frame since they are headers
        channels[ch] = ((ibus_frame[(ch * 2) + 1] & 0x0F) << 8) | ibus_frame[(ch * 2)];
        checksum += ibus_frame[(ch * 2)] + ibus_frame[(ch * 2) + 1];
    }

    // 30, 31 are checksum bytes
    checksum += (ibus_frame[IBUS_FRAME_LENGTH - 1] << 8) | ibus_frame[IBUS_FRAME_LENGTH - 2];

    /* 0xFFFF - sum = checksum*/
    if (checksum != 0xFFFF){
        return false;
    }

    ibus_failsafe = (ibus_frame[3]&0xF0) | (ibus_frame[9]&0xF0) ? true : false; // does not works

    return true;
}


void ibus_handle_byte(uint8_t byte, uint32_t time_ms)
{
    static uint8_t ibus_frame[IBUS_FRAME_LENGTH] = {0};
    static uint32_t last_time_ms = 0;
    static uint8_t byte_count = 0;

    bool new_frame = (time_ms - last_time_ms >= 2) ? true : false; // need 2ms frame gap
    last_time_ms = time_ms; // load byte timestamp

    if (new_frame){
        byte_count = 0;
    }

    if (byte_count == 0 && byte != IBUS_HEADER_LOW){
        // header mismatch
        return;
    }

    ibus_frame[byte_count++] = byte;

    if (byte_count == IBUS_FRAME_LENGTH)
    {
        uint16_t channels[IBUS_NUM_OF_CHANNELS + 1] = {0};
        if (decode(ibus_frame, channels)){
            // Successfull decoding
            memcpy(rc_channels, channels, sizeof(channels));
        }

        // reset frame buffer and counter
        memset(ibus_frame, 0, sizeof(ibus_frame));
        byte_count = 0;
    }
}

