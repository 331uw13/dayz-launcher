#ifndef A2S_CHUNK_H
#define A2S_CHUNK_H

#include <stdint.h>

struct a2s_chunk_t {

    uint8_t index;
    uint8_t total;

    char data[125];
    size_t data_len;

};





#endif
