#include <stdio.h>

#include "a2s_decoder.h"





bool decode_a2s_escapes(uint8_t* buffer, size_t size, uint8_t* output, size_t output_max) {

    if(size > output_max) {
        fprintf(stderr, "%s: Output(decoded) buffer should not be smaller than the original buffer.\n",
                __func__);
        return false;
    }

    //              Steam header --.    | v-- "this is A2S rules response"
    //                             |    |  | v-- Number of rules.
    //                             v    |  |  | v-- Chunk index.
    //                                  |  |  |  | v-- Chunks total.
    // Ignore first 8 bytes:|FF FF FF FF|xx|xx|xx|xx| 

    // Escape sequence
    // 0x01 0x01  ->  0x01
    // 0x01 0x02  ->  0x00
    // 0x01 0x03  ->  0xFF

    size_t out_idx = 0;
    uint8_t* byte = &buffer[8];

    while(byte < (buffer + size - 1)) {

        uint8_t next_byte = *(byte + 1);

        if((*byte == 0x01) && (next_byte == 0x01)) {
            output[out_idx] = 0x01;
            byte += 2;
        }
        else
        if((*byte == 0x01) && (next_byte == 0x02)) {
            output[out_idx] = 0x00;
            byte += 2;
        }
        else
        if((*byte == 0x01) && (next_byte == 0x03)) {
            output[out_idx] = 0xFF;
            byte += 2;
        }
        else {
            output[out_idx] = *byte;
            byte++;
        }

        out_idx++;
    }

    return true;
}





